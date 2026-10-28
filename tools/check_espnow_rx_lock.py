#!/usr/bin/env python3
"""Exercise actual ESP-NOW packet functions with deterministic two-thread scheduling.

Requires a POSIX C compiler. Kernel critical sections are mapped to pthread
mutexes; packet and ring-buffer algorithms are extracted unchanged from source.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
import tempfile


def function(text, name):
    start = re.search(r'^[\w *]+\b' + re.escape(name) + r'\([^;]*?\)\s*\{', text, re.M).start()
    opening = text.index('{', start)
    depth, end = 1, opening + 1
    while depth:
        depth += (text[end] == '{') - (text[end] == '}')
        end += 1
    return text[start:end]


PREFIX = r'''
#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <setjmp.h>
#include <pthread.h>
#include <stdatomic.h>
#include <sched.h>
#define ESPNOW_MAGIC 0x99
#define ESP_NOW_ETH_ALEN 6
#define ESP_NOW_MAX_DATA_LEN 250
#define m_malloc malloc
#define portMUX_TYPE pthread_mutex_t
#define portMUX_INITIALIZER_UNLOCKED PTHREAD_MUTEX_INITIALIZER
static _Thread_local int locked, is_worker;
static atomic_int worker_attempted, worker_go, worker_done;
static void enter(pthread_mutex_t *m) {
 if (is_worker) atomic_store(&worker_attempted,1);
 assert(pthread_mutex_lock(m)==0); locked++;
}
static void leave(pthread_mutex_t *m) {
 assert(locked==1); locked--; assert(pthread_mutex_unlock(m)==0);
}
#define portENTER_CRITICAL(m) enter(m)
#define portEXIT_CRITICAL(m) leave(m)
typedef uintptr_t mp_obj_t;
typedef int mp_obj_type_t;
static int espnow_packet_type_obj;
#define mp_const_none ((mp_obj_t)0)
#define MP_QSTR_buffer 1
#define MP_OBJ_NEW_SMALL_INT(v) ((mp_obj_t)(v))
static _Thread_local jmp_buf error_target;
static _Thread_local uint8_t got_mac[6],got_payload[250];
static _Thread_local size_t got_length;
static _Thread_local int bytes_index;
static void mp_arg_error_invalid(int q) { (void)q; assert(!locked); longjmp(error_target,1); }
static mp_obj_t mp_obj_new_bytes(const uint8_t *p,size_t n) {
 assert(!locked);
 if (bytes_index++==0) { assert(n==6); memcpy(got_mac,p,n); }
 else { assert(n<=250);memcpy(got_payload,p,n);got_length=n; }
 return 1;
}
static mp_obj_t mp_obj_new_int(uint32_t n) { assert(!locked);return n; }
static mp_obj_t namedtuple_make_new(const mp_obj_type_t *t,size_t n,size_t k,const mp_obj_t *e) {
 (void)t;(void)n;(void)k;(void)e;assert(!locked);return 1;
}
static uint32_t mp_hal_ticks_ms(void) { return 123; }
typedef struct { int8_t rssi; } rx_ctrl_t;
typedef struct { const uint8_t *src_addr; rx_ctrl_t *rx_ctrl; } esp_now_recv_info_t;
'''

MIDDLE = r'''
typedef struct { ringbuf_t *recv_buffer;unsigned read_failure,read_success; } espnow_obj_t;
static espnow_obj_t obj;
#define MP_STATE_PORT(x) (&obj)
static int inject_put,inject_get;
static void scheduling_point(void) {
 atomic_store(&worker_go,1);
 // On the fixed source the worker reaches the lock but cannot enter. On the
 // baseline it finishes reading the partial packet before the writer resumes.
 while (!atomic_load(&worker_attempted) && !atomic_load(&worker_done)) sched_yield();
}
static size_t scheduling_put(ringbuf_t *r,const uint8_t *b,size_t n) {
 size_t result=ringbuf_put_n(r,b,n);
 if (inject_put && !is_worker) { inject_put=0;scheduling_point(); }
 return result;
}
static size_t scheduling_get(ringbuf_t *r,uint8_t *b,size_t n) {
 size_t result=ringbuf_get_n(r,b,n);
 if (inject_get && !is_worker) { inject_get=0;scheduling_point(); }
 return result;
}
#define ringbuf_put_n scheduling_put
#define ringbuf_get_n scheduling_get
'''

MAIN = r'''
#undef ringbuf_put_n
#undef ringbuf_get_n
static uint8_t storage[1024],mac[6]={2,3,4,5,6,7},payload[250];
static ringbuf_t rb;
static rx_ctrl_t rx={-30};
static esp_now_recv_info_t info={mac,&rx};
static int worker_action,worker_result;
static size_t worker_available;
static void reset_case(size_t capacity) {
 assert(!locked);ringbuf_init(&rb,storage,capacity);obj=(espnow_obj_t){.recv_buffer=&rb};
 inject_put=inject_get=0;atomic_store(&worker_go,0);atomic_store(&worker_done,0);
 atomic_store(&worker_attempted,0);worker_result=0;worker_available=0;
}
static int safe_read(void) {
 bytes_index=0;
 if (setjmp(error_target)) return -1;
 return common_hal_espnow_read(&obj) ? 1 : 0;
}
static void check_payload(size_t length) {
 assert(got_length==length && memcmp(got_mac,mac,6)==0);
 assert(memcmp(got_payload,payload,length)==0);
}
static void *worker(void *unused) {
 (void)unused;is_worker=1;
 while(!atomic_load(&worker_go)) sched_yield();
 if(worker_action==0) { worker_result=safe_read();if(worker_result==1) check_payload(20); }
 else if(worker_action==1) recv_cb(&info,payload,20);
 else worker_available=common_hal_espnow_get_bytes_available(&obj);
 assert(!locked);atomic_store(&worker_done,1);return NULL;
}
static void start_worker(pthread_t *t,int action) {
 worker_action=action;assert(pthread_create(t,NULL,worker,NULL)==0);
}
static atomic_int producer_done;
static unsigned delivered;
static void *producer(void *unused) {
 (void)unused;
 uint8_t p[250];
 for(uint32_t seq=1;seq<=20000;seq++) {
  memcpy(p,&seq,4);for(size_t k=4;k<sizeof(p);k++) p[k]=(uint8_t)(seq+k);
  recv_cb(&info,p,sizeof(p));
  if(seq%11==0)sched_yield();
 }
 atomic_store(&producer_done,1);return NULL;
}
static void *consumer(void *unused) {
 (void)unused;uint32_t previous=0;
 while(!atomic_load(&producer_done) || common_hal_espnow_get_bytes_available(&obj)) {
  int result=safe_read();assert(result>=0);
  if(result) {
   assert(got_length==250 && memcmp(got_mac,mac,6)==0);
   uint32_t seq;memcpy(&seq,got_payload,4);assert(seq>previous && seq<=20000);previous=seq;
   for(size_t k=4;k<250;k++)assert(got_payload[k]==(uint8_t)(seq+k));
   delivered++;
  } else sched_yield();
 }
 return NULL;
}
int main(void) {
 memset(payload,0x42,sizeof(payload));pthread_t t;
 reset_case(sizeof(storage));assert(safe_read()==0);assert(!locked);
 puts("PASS empty_read_unlocks");
 for(size_t n=0;n<=250;n+=(n==0?1:249)) {
  reset_case(sizeof(espnow_packet_t)+n);recv_cb(&info,payload,n);
  assert(safe_read()==1);check_payload(n);assert(rb.used==0 && obj.read_success==1);
 }
 puts("PASS zero_one_maximum_payload_exact_capacity");
 reset_case(sizeof(espnow_packet_t)+19);recv_cb(&info,payload,20);
 assert(obj.read_failure==1 && rb.used==0 && !locked);
 puts("PASS insufficient_capacity_drops_whole_packet_and_unlocks");
 reset_case(sizeof(storage));start_worker(&t,0);inject_put=1;
 recv_cb(&info,payload,20);assert(pthread_join(t,NULL)==0);
 if(EXPECT_RACE) {
  assert(worker_result==-1 && safe_read()==-1);
  puts("REPRODUCED partial_header_read_and_corrupt_followup_on_baseline");return 0;
 }
 assert(worker_result==1 && rb.used==0 && obj.read_success==1);
 puts("PASS reader_waits_for_complete_packet");
 reset_case(sizeof(storage));start_worker(&t,2);inject_put=1;
 recv_cb(&info,payload,20);assert(pthread_join(t,NULL)==0);
 assert(worker_available==sizeof(espnow_packet_t)+20 && safe_read()==1);
 puts("PASS readiness_waits_for_complete_packet");
 reset_case(sizeof(storage));recv_cb(&info,payload,20);start_worker(&t,1);inject_get=1;
 assert(safe_read()==1);check_payload(20);assert(pthread_join(t,NULL)==0);
 assert(safe_read()==1);check_payload(20);assert(rb.used==0);
 puts("PASS producer_waits_for_complete_dequeue");
 reset_case(sizeof(espnow_packet_t)+250);
 for(int i=0;i<400;i++) {
  size_t n=(size_t)(i%251);recv_cb(&info,payload,n);assert(safe_read()==1);check_payload(n);
 }
 puts("PASS wraparound_variable_lengths");
 reset_case(sizeof(storage));recv_cb(&info,payload,-1);recv_cb(&info,payload,251);
 assert(obj.read_failure==2 && rb.used==0 && !locked);
 puts("PASS_invalid_callback_lengths_drop_and_unlock");
 for(int corrupt=0;corrupt<4;corrupt++) {
  reset_case(sizeof(storage));recv_cb(&info,payload,20);
  if(corrupt==0)storage[0]=0;
  if(corrupt==1)storage[1]=251;
  if(corrupt==2)rb.used=2;
  if(corrupt==3)rb.used=sizeof(espnow_packet_t)+19;
  assert(safe_read()==-1 && !locked);
 }
 puts("PASS malformed_header_and_truncation_raise_after_unlock");
 reset_case(sizeof(storage));atomic_store(&producer_done,0);delivered=0;
 pthread_t p,c;assert(pthread_create(&c,NULL,consumer,NULL)==0);
 assert(pthread_create(&p,NULL,producer,NULL)==0);
 assert(pthread_join(p,NULL)==0 && pthread_join(c,NULL)==0);
 assert(delivered>0 && delivered==obj.read_success && delivered+obj.read_failure==20000 && rb.used==0);
 printf("PASS concurrent_20000_packets_integrity_order_accounting delivered=%u dropped=%u\n",delivered,obj.read_failure);
 return 0;
}
'''


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('circuitpython',type=Path)
    parser.add_argument('--revision')
    parser.add_argument('--expect-race',action='store_true')
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args();root=args.circuitpython.resolve()
    files=['ports/espressif/common-hal/espnow/ESPNow.c','py/ringbuf.c','py/ringbuf.h']
    def read(p):
        if args.revision:
            return subprocess.check_output(['git','-C',str(root),'show',f'{args.revision}:{p}'],text=True)
        return (root/p).read_text()
    sources={p:read(p) for p in files};esp,ring,header=(sources[p] for p in files)
    header=header.replace('#include "py/gc.h"','').replace('#pragma once','')
    ring=ring.replace('#include "ringbuf.h"','')
    types=esp[esp.index('typedef struct {'):esp.index('// --- The ESP-NOW send')]
    available='common_hal_espnow_get_bytes_available'
    getter=function(esp,available) if available in esp else f'size_t {available}(espnow_obj_t *self) {{return ringbuf_num_filled(self->recv_buffer);}}'
    code=PREFIX+header+types+ring+MIDDLE+getter+function(esp,'common_hal_espnow_read')+function(esp,'recv_cb')+MAIN
    code=f'#define EXPECT_RACE {int(args.expect_race)}\n'+code
    with tempfile.TemporaryDirectory(prefix='espnow-rx-lock-') as temp:
        src=Path(temp)/'check.c';exe=Path(temp)/'check';src.write_text(code)
        subprocess.run(['cc','-std=c11','-O2','-Wall','-Wextra','-Werror','-Wno-unused-function','-pthread',str(src),'-o',str(exe)],check=True)
        result=subprocess.run([str(exe)],capture_output=True,text=True,timeout=30)
    data=dict(revision=args.revision or 'working-tree',source_sha256={p:hashlib.sha256(v.encode()).hexdigest() for p,v in sources.items()},generated_c_sha256=hashlib.sha256(code.encode()).hexdigest(),expected_race=args.expect_race,returncode=result.returncode,checks=result.stdout.splitlines(),stderr=result.stderr,scope='Actual packet/ring-buffer source with pthread critical sections, deterministic scheduling and concurrent integrity tests; not physical incidence data')
    args.output.write_text(json.dumps(data,indent=2)+'\n');print(json.dumps(data,indent=2))
    raise SystemExit(result.returncode)


if __name__=='__main__':main()
