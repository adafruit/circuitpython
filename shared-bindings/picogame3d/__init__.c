// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT
//
// picogame3d: perspective and 3D drawing for picogame canvases.

#include <string.h>

#include "py/runtime.h"
#include "shared-module/picogame/pg_compat.h"
#include "shared-bindings/picogame/Bitmap.h"
#include "shared-bindings/picogame/Canvas.h"
#include "shared-module/picogame/__init__.h"
#include "shared-module/picogame3d/__init__.h"

//| """Perspective and 3D drawing for picogame
//|
//| The `picogame3d` module draws pseudo-3D views into a :py:class:`picogame.Canvas`,
//| including a `picogame.StripDraw` view: a perspective floor (`mode7`), a racing
//| road (`road_edges` and `road`) and grid-map wall rays (`raycast`). `project`
//| batch-projects 3D points, and `project_mesh` projects, culls and sorts a whole
//| triangle mesh, for :py:class:`picogame.Triangles` and
//| :py:meth:`picogame.Canvas.fill_triangles`.
//|
//| These are low-level interfaces. The ``picogame_mode7``, ``picogame_road`` and
//| ``picogame_ray`` helper libraries compute their inputs from camera parameters."""
//|
//| FLOAT: int
//| """``1`` when `project` uses hardware floating point (its buffers are
//| ``float32``), ``0`` when it uses signed 16.16 fixed-point ``int32``."""
//|

static picogame_canvas_obj_t *pg3d_canvas(mp_obj_t o) {
    return MP_OBJ_TO_PTR(mp_arg_validate_type(o, &picogame_canvas_type, MP_QSTR_canvas));
}

//| def mode7(
//|     canvas: picogame.Canvas,
//|     texture: picogame.Bitmap,
//|     horizon: int,
//|     y_off: int,
//|     z: int,
//|     rx0: int,
//|     ry0: int,
//|     rsx: int,
//|     rsy: int,
//|     cam_x: int,
//|     cam_y: int,
//| ) -> None:
//|     """Fill rows of ``canvas`` below ``horizon`` with a perspective projection of
//|     ``texture``. The ``picogame_mode7`` helper computes the camera terms from an
//|     angle, position and field of view.
//|
//|     :param picogame.Canvas canvas: surface to draw into
//|     :param picogame.Bitmap texture: texture whose width and height are powers of two
//|     :param int horizon: first canvas row to fill
//|     :param int y_off: vertical offset of the projection, in rows
//|     :param int z: camera height (signed 16.16 fixed-point)
//|     :param int rx0: ray x at the left column (signed 16.16 fixed-point)
//|     :param int ry0: ray y at the left column (signed 16.16 fixed-point)
//|     :param int rsx: per-column ray step x (signed 16.16 fixed-point)
//|     :param int rsy: per-column ray step y (signed 16.16 fixed-point)
//|     :param int cam_x: camera x position (signed 16.16 fixed-point)
//|     :param int cam_y: camera y position (signed 16.16 fixed-point)"""
//|     ...
//|
static mp_obj_t picogame3d_mode7_fn(size_t n, const mp_obj_t *a) {
    picogame_canvas_obj_t *cv = pg3d_canvas(a[0]);
    picogame_bitmap_obj_t *tex = MP_OBJ_TO_PTR(
        mp_arg_validate_type(a[1], &picogame_bitmap_type, MP_QSTR_texture));
    picogame3d_mode7(cv, tex,
        mp_obj_get_int(a[2]), mp_obj_get_int(a[3]), mp_obj_get_int(a[4]),
        mp_obj_get_int(a[5]), mp_obj_get_int(a[6]), mp_obj_get_int(a[7]),
        mp_obj_get_int(a[8]), mp_obj_get_int(a[9]), mp_obj_get_int(a[10]));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picogame3d_mode7_obj, 11, 11, picogame3d_mode7_fn);

//| def road(
//|     canvas: picogame.Canvas,
//|     ri0: int,
//|     tab: ReadableBuffer,
//|     rl: ReadableBuffer,
//|     rr: ReadableBuffer,
//|     d05_q8: int,
//|     d07_q8: int,
//|     colors: ReadableBuffer,
//| ) -> None:
//|     """Draw one racing-road strip into ``canvas`` from precomputed tables. A
//|     low-level interface used by the ``picogame_road`` helper, which builds the tables.
//|
//|     :param picogame.Canvas canvas: surface to draw into
//|     :param int ri0: road-table row of the canvas's row 0; negative values
//|         are sky rows
//|     :param ~circuitpython_typing.ReadableBuffer tab: ``int16`` rows of
//|         ``edge_w``, ``dash_hw``, ``wb05_q8``, ``wb07_q8``, ``flags``
//|     :param ~circuitpython_typing.ReadableBuffer rl: ``int16`` per-row left
//|         edges, as computed by :py:func:`road_edges`
//|     :param ~circuitpython_typing.ReadableBuffer rr: ``int16`` per-row right
//|         edges
//|     :param int d05_q8: scroll phase (Q8 fixed-point)
//|     :param int d07_q8: scroll phase (Q8 fixed-point)
//|     :param ~circuitpython_typing.ReadableBuffer colors: six ``uint16`` colors,
//|         in order: sky, road a, road b, rumble a, rumble b, dash"""
//|     ...
//|
static mp_obj_t picogame3d_road_fn(size_t n, const mp_obj_t *a) {
    picogame_canvas_obj_t *cv = pg3d_canvas(a[0]);
    mp_buffer_info_t tabi, rli, rri, coli;
    mp_get_buffer_raise(a[2], &tabi, MP_BUFFER_READ);
    mp_get_buffer_raise(a[3], &rli, MP_BUFFER_READ);
    mp_get_buffer_raise(a[4], &rri, MP_BUFFER_READ);
    mp_get_buffer_raise(a[7], &coli, MP_BUFFER_READ);
    int ntab = (int)(tabi.len / (5 * 2));
    int nedge = (int)(rli.len < rri.len ? rli.len : rri.len) / 2;
    if (nedge < ntab) {
        ntab = nedge;                                // never read past the shorter per-frame arrays
    }
    if (ntab <= 0 || coli.len < 6 * 2) {
        return mp_const_none;
    }
    picogame3d_road(cv, mp_obj_get_int(a[1]),
        (const int16_t *)tabi.buf, ntab, (const int16_t *)rli.buf, (const int16_t *)rri.buf,
        mp_obj_get_int(a[5]), mp_obj_get_int(a[6]), (const uint16_t *)coli.buf);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picogame3d_road_obj, 8, 8, picogame3d_road_fn);

#if !CIRCUITPY_PICOGAME3D_FLOAT
// ((int64_t)a * b) >> 16 with two 32-bit multiplies. Exact for |b| <= 65536.
static __attribute__((noinline)) int32_t pg_fmul_basis(int32_t a, int32_t b) {
    uint32_t hi = (uint32_t)(a >> 16) * (uint32_t)b;      // modulo 2^32 on purpose
    uint32_t l = (uint32_t)(a & 0xFFFF);
    if (b >= 0) {
        return (int32_t)(hi + ((l * (uint32_t)b) >> 16));
    }
    return (int32_t)(hi - ((l * (uint32_t)(-b) + 0xFFFFu) >> 16));
}
#endif

#if CIRCUITPY_PICOGAME3D_FLOAT
typedef float pg_num_t;
#else
typedef int32_t pg_num_t;
#endif

// Project n points for project and project_mesh. Screen x and y advance by `step` int16 slots and
// the view depth by `zstep` slots, so the outputs can be separate arrays or fields of a struct.
// A point behind the near plane gets x = y = -32768.
static __attribute__((noinline)) void pg_project_points(const pg_num_t *cam, const pg_num_t *pts, int n,
    int16_t *sx, int16_t *sy, int step, pg_num_t *sz, int zstep) {
    pg_num_t ex = cam[0], ey = cam[1], ez = cam[2];
    pg_num_t rx = cam[3], rz = cam[4];
    pg_num_t ux = cam[5], uy = cam[6], uz = cam[7];
    pg_num_t fx = cam[8], fy = cam[9], fz = cam[10];
    pg_num_t focal = cam[11], cx0 = cam[12], cy0 = cam[13], near = cam[14];
    #if !CIRCUITPY_PICOGAME3D_FLOAT
    // Full-precision Q16 dot products (int64 mul per term). A Q8-prescaled-basis/MULS variant was
    // ~30% faster, but its error grows with |coord| (~0.2%/axis) and k = focal/cz AMPLIFIES it near
    // the near plane - host-measured 23-34 px warps on close fly-bys at a file-browser world scale
    // (walls visibly broke). Correctness first: Q16 keeps the worst error a few px at any cz >= near,
    // for coords up to +-32k units; still ~4-5x faster than the same math in Python on the M0+.
    // FMULB: same result as FMUL for |b| <= 65536 (every basis component), without
    // __aeabi_lmul. The products by k keep FMUL because k is not bounded.
    #define FMUL(a, b) ((int32_t)(((int64_t)(a) * (b)) >> 16))
    #define FMULB(a, b) pg_fmul_basis((a), (b))
    #endif
    for (int i = 0; i < n; i++) {
        pg_num_t X = pts[i * 3] - ex, Y = pts[i * 3 + 1] - ey, Z = pts[i * 3 + 2] - ez;
        int16_t x = -32768, y = -32768;
        #if CIRCUITPY_PICOGAME3D_FLOAT
        float cz = X * fx + Y * fy + Z * fz;
        if (cz >= near) {
            float k = focal / cz;                   // hardware divide on an FPU part
            x = (int16_t)(cx0 + (X * rx + Z * rz) * k);
            y = (int16_t)(cy0 - (X * ux + Y * uy + Z * uz) * k);
        }
        #else
        int32_t cz = FMULB(X, fx) + FMULB(Y, fy) + FMULB(Z, fz);
        if (cz >= near) {
            // focal/cz in 16.16. A 32-bit divide (focal<<8 = Q24, cz>>8 = Q8 -> Q16) is ~4x cheaper
            // than an int64 divide on the M0+ (no HW divide) and the lost cz precision costs <0.02 px
            // (host-measured). Needs FOCAL < ~250 (focal<<8 in uint32) and near >= 1/256 (cz>>8 nonzero).
            int32_t k = (int32_t)(((uint32_t)focal << 8) / (uint32_t)(cz >> 8));
            int32_t rr = FMULB(X, rx) + FMULB(Z, rz);
            int32_t uu = FMULB(X, ux) + FMULB(Y, uy) + FMULB(Z, uz);
            x = (int16_t)((cx0 + FMUL(rr, k)) >> 16);
            y = (int16_t)((cy0 - FMUL(uu, k)) >> 16);
        }
        #endif
        *sx = x;
        *sy = y;
        *sz = cz;
        sx += step;
        sy += step;
        sz += zstep;
    }
#undef FMUL
#undef FMULB
}

//| def project(
//|     cam: ReadableBuffer,
//|     pts: ReadableBuffer,
//|     n: int,
//|     out_sx: WriteableBuffer,
//|     out_sy: WriteableBuffer,
//| ) -> None:
//|     """Batch-project ``n`` 3D world points to screen coordinates.
//|
//|     When :py:data:`FLOAT` is ``1``, ``cam`` and ``pts`` hold ``float32`` elements;
//|     otherwise they hold signed 16.16 fixed-point ``int32`` elements.
//|
//|     :param ~circuitpython_typing.ReadableBuffer cam: 15 camera parameters, in order:
//|         eye x/y/z, right x/z (the camera cannot roll, so right has no y
//|         component), up x/y/z, forward x/y/z, focal length, screen center x/y,
//|         near-plane distance
//|     :param ~circuitpython_typing.ReadableBuffer pts: ``3 * n`` world coordinates
//|         (x, y, z per point)
//|     :param int n: number of points; limited to what ``pts``, ``out_sx`` and
//|         ``out_sy`` hold
//|     :param ~circuitpython_typing.WriteableBuffer out_sx: at least ``n`` ``int16``
//|         values; receives screen x, or the sentinel ``-32768`` for a point behind
//|         the near plane
//|     :param ~circuitpython_typing.WriteableBuffer out_sy: at least ``n`` ``int16``
//|         values; receives screen y, or ``-32768`` for a point behind the near plane"""
//|     ...
//|
// project(cam, pts, n, out_sx, out_sy) - batch perspective projection of `n` 3D points to screen.
//   cam  = 15 camera params: ex,ey,ez, rx,rz, ux,uy,uz, fx,fy,fz, focal, cx0, cy0, near
//   pts  = n*3 world coords (x,y,z per point)
//   out_sx/out_sy = int16 screen coords; a point behind the near plane gets sentinel -32768
// On an FPU board (CIRCUITPY_PICOGAME3D_FLOAT) cam/pts are float32; else they are 16.16 fixed int32.
// This is the shared hot path for blocky pseudo-3D (project the 8 corners of each box, then fill).
static mp_obj_t picogame3d_project(size_t n_args, const mp_obj_t *args) {
    mp_buffer_info_t ci, pi, xi, yi;
    mp_get_buffer_raise(args[0], &ci, MP_BUFFER_READ);
    mp_get_buffer_raise(args[1], &pi, MP_BUFFER_READ);
    int n = mp_obj_get_int(args[2]);
    mp_get_buffer_raise(args[3], &xi, MP_BUFFER_WRITE);
    mp_get_buffer_raise(args[4], &yi, MP_BUFFER_WRITE);
    int16_t *osx = xi.buf;
    int16_t *osy = yi.buf;
    mp_arg_validate_length_min(ci.len, 15 * 4, MP_QSTR_cam);
    // Never read or write past any buffer: 3 four-byte coordinates per point, one int16 per output.
    n = picogame_imin(n, (int)(pi.len / 12));
    n = picogame_imin(n, (int)(picogame_imin(xi.len, yi.len) >> 1));
    pg_num_t depth;                                 // not wanted here
    pg_project_points(ci.buf, pi.buf, n, osx, osy, 1, &depth, 0);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picogame3d_project_obj, 5, 5, picogame3d_project);

//| def project_mesh(
//|     cam: ReadableBuffer,
//|     pts: ReadableBuffer,
//|     n: int,
//|     colors: ReadableBuffer,
//|     out_verts: WriteableBuffer,
//|     out_colors: WriteableBuffer,
//|     scratch: WriteableBuffer,
//|     indices: ReadableBuffer,
//| ) -> int:
//|     """Project a triangle mesh and prepare it for :py:meth:`picogame.Canvas.fill_triangles`
//|     or :py:class:`picogame.Triangles`.
//|
//|     Triangles that cross the near plane or face away from the camera are dropped. The rest
//|     are written far to near, so drawing them in order hides the parts behind. ``cam`` and
//|     ``pts`` use the same element types as `project`.
//|
//|     :param ~circuitpython_typing.ReadableBuffer cam: 15 camera parameters, as for
//|         `project`
//|     :param ~circuitpython_typing.ReadableBuffer pts: world coordinates, x, y, z per point
//|     :param int n: number of triangles
//|     :param ~circuitpython_typing.ReadableBuffer colors: ``n`` ``uint16`` colors, one per triangle
//|     :param ~circuitpython_typing.WriteableBuffer out_verts: ``int16`` screen coordinates, six
//|         per output triangle. It may hold fewer than ``n`` triangles, because only the ones
//|         facing the camera are written
//|     :param ~circuitpython_typing.WriteableBuffer out_colors: ``uint16`` colors, one per output
//|         triangle
//|     :param ~circuitpython_typing.WriteableBuffer scratch: working space: 8 bytes per output
//|         triangle plus 8 bytes per point
//|     :param ~circuitpython_typing.ReadableBuffer indices: ``uint16`` point indices, three per
//|         triangle; each point is projected once, however many triangles share it
//|     :return: the number of triangles written
//|     :raises ValueError: if a buffer is too small, including when more triangles face the
//|         camera than ``out_verts`` or ``out_colors`` can hold, or if an index is past the
//|         last point"""
//|     ...
//|
typedef struct {
    int16_t x, y;
    pg_num_t z;                                     // view depth; below near = behind the camera
} pg_vertex_t;

// Sort the m output triangles by key (depth rank << 16 | slot) and reorder verts/colors to match.
static void pg_mesh_order(uint32_t *key, uint32_t *tmp, int m, int16_t *verts, uint16_t *colors) {
    uint16_t count[256];
    // The slot in the low half makes every key unique, so only the two rank bytes need a pass.
    for (int shift = 16; shift < 32; shift += 8) {
        memset(count, 0, sizeof(count));
        for (int i = 0; i < m; i++) {
            count[(key[i] >> shift) & 255]++;
        }
        uint16_t pos = 0;
        for (int b = 0; b < 256; b++) {
            uint16_t c = count[b];
            count[b] = pos;
            pos += c;
        }
        for (int i = 0; i < m; i++) {
            tmp[count[(key[i] >> shift) & 255]++] = key[i];
        }
        uint32_t *swap = key;
        key = tmp;
        tmp = swap;
    }
    // Two passes leave the result back in key. Apply the permutation in place, one cycle at a
    // time; 0xffffffff marks a finished slot (m <= 65535 keeps real keys below it).
    for (int i = 0; i < m; i++) {
        if (key[i] == 0xffffffff) {
            continue;
        }
        int16_t v[6];
        memcpy(v, verts + i * 6, sizeof(v));
        uint16_t c = colors[i];
        int j = i;
        for (;;) {
            int k = key[j] & 0xffff;
            key[j] = 0xffffffff;
            if (k == i) {
                break;
            }
            memcpy(verts + j * 6, verts + k * 6, sizeof(v));
            colors[j] = colors[k];
            j = k;
        }
        memcpy(verts + j * 6, v, sizeof(v));
        colors[j] = c;
    }
}

static mp_obj_t picogame3d_project_mesh(size_t n_args, const mp_obj_t *args) {
    mp_buffer_info_t cam_info, pts_info, colors_info, verts_info, out_colors_info, scratch_info;
    mp_buffer_info_t indices_info;
    mp_get_buffer_raise(args[0], &cam_info, MP_BUFFER_READ);
    mp_get_buffer_raise(args[1], &pts_info, MP_BUFFER_READ);
    int n = mp_obj_get_int(args[2]);
    mp_get_buffer_raise(args[3], &colors_info, MP_BUFFER_READ);
    mp_get_buffer_raise(args[4], &verts_info, MP_BUFFER_WRITE);
    mp_get_buffer_raise(args[5], &out_colors_info, MP_BUFFER_WRITE);
    mp_get_buffer_raise(args[6], &scratch_info, MP_BUFFER_WRITE);
    mp_get_buffer_raise(args[7], &indices_info, MP_BUFFER_READ);
    int point_count = (int)(pts_info.len / (3 * sizeof(pg_num_t)));
    int capacity = (int)MIN(verts_info.len / 12, out_colors_info.len / 2);
    capacity = MIN(capacity, n);
    size_t scratch_needed = (size_t)capacity * 8 + (size_t)point_count * sizeof(pg_vertex_t);
    if (n < 0 || n > 65535 || cam_info.len < 15 * sizeof(pg_num_t) || colors_info.len < (size_t)n * 2
        || scratch_info.len < scratch_needed
        || indices_info.len < (size_t)n * 6) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffer too small"));
    }
    pg_num_t cam[15];                               // a local copy, so the output stores cannot alias it
    memcpy(cam, cam_info.buf, sizeof(cam));
    const uint16_t *indices = indices_info.buf;
    const uint16_t *colors = colors_info.buf;
    int16_t *verts = verts_info.buf;
    uint16_t *out_colors = out_colors_info.buf;
    uint32_t *key = scratch_info.buf;
    // Every point is projected once, after the 8 bytes per triangle that the sort needs.
    pg_vertex_t *projected = (pg_vertex_t *)(key + 2 * capacity);
    pg_project_points(cam, pts_info.buf, point_count, &projected->x, &projected->y,
        sizeof(pg_vertex_t) / 2, &projected->z, sizeof(pg_vertex_t) / sizeof(pg_num_t));
    // First pass: keep the visible triangles and their summed depth, then rank the depths.
    pg_num_t *depth = (pg_num_t *)key;
    pg_num_t nearest = 0, farthest = 0;
    int m = 0;
    for (int t = 0; t < n; t++) {
        const pg_vertex_t *v[3];
        pg_num_t d = 0;
        int j;
        for (j = 0; j < 3; j++) {
            int i = indices[t * 3 + j];
            if (i >= point_count) {
                mp_raise_ValueError(MP_ERROR_TEXT("index out of range"));
            }
            v[j] = projected + i;
            if (v[j]->z < cam[14]) {
                break;
            }
            #if CIRCUITPY_PICOGAME3D_FLOAT
            d += v[j]->z;
            #else
            d += v[j]->z >> 2;                      // three depths summed without overflow
            #endif
        }
        // Skip a triangle that crosses the near plane or faces away (screen y grows downward).
        if (j < 3 || (int32_t)(v[1]->x - v[0]->x) * (v[2]->y - v[0]->y)
            >= (int32_t)(v[1]->y - v[0]->y) * (v[2]->x - v[0]->x)) {
            continue;
        }
        if (m == capacity) {
            mp_raise_ValueError(MP_ERROR_TEXT("buffer too small"));
        }
        for (j = 0; j < 3; j++) {
            verts[m * 6 + j * 2] = v[j]->x;
            verts[m * 6 + j * 2 + 1] = v[j]->y;
        }
        if (m == 0 || d < nearest) {
            nearest = d;
        }
        if (m == 0 || d > farthest) {
            farthest = d;
        }
        depth[m] = d;
        out_colors[m++] = colors[t];
    }
    #if CIRCUITPY_PICOGAME3D_FLOAT
    float scale = farthest > nearest ? 65535.0f / (farthest - nearest) : 0;
    for (int i = 0; i < m; i++) {
        key[i] = (uint32_t)((farthest - depth[i]) * scale) << 16 | i;
    }
    #else
    int shift = 0;
    while ((uint32_t)(farthest - nearest) >> shift > 65535) {
        shift++;
    }
    for (int i = 0; i < m; i++) {
        key[i] = ((uint32_t)(farthest - depth[i]) >> shift) << 16 | i;
    }
    #endif
    pg_mesh_order(key, key + capacity, m, verts, out_colors);
    return MP_OBJ_NEW_SMALL_INT(m);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picogame3d_project_mesh_obj, 8, 8, picogame3d_project_mesh);

//| def raycast(
//|     map: ReadableBuffer,
//|     mw: int,
//|     mh: int,
//|     posx: int,
//|     posy: int,
//|     lrx: int,
//|     lry: int,
//|     srx: int,
//|     sry: int,
//|     sh: int,
//|     stride: int,
//|     ncols: int,
//|     wcolors: ReadableBuffer,
//|     top: WriteableBuffer,
//|     bot: WriteableBuffer,
//|     col: WriteableBuffer,
//|     dist: WriteableBuffer,
//|     runs: Optional[WriteableBuffer] = None,
//| ) -> Optional[int]:
//|     """Cast one frame of wall-finding rays across the screen columns of a
//|     grid-map view. A low-level interface used by the ``picogame_ray`` helper,
//|     which computes these inputs.
//|
//|     All fixed-point arguments are signed 16.16 integers.
//|
//|     :param ~circuitpython_typing.ReadableBuffer map: at least ``mw * mh`` wall-type
//|         bytes, row-major; 0 is empty
//|     :param int mw: map width in cells
//|     :param int mh: map height in cells
//|     :param int posx: camera x position (fixed-point)
//|     :param int posy: camera y position (fixed-point)
//|     :param int lrx: x of the column-0 ray direction (fixed-point)
//|     :param int lry: y of the column-0 ray direction (fixed-point)
//|     :param int srx: per-column ray step x (fixed-point)
//|     :param int sry: per-column ray step y (fixed-point)
//|     :param int sh: screen height in pixels
//|     :param int stride: pixel width of one column
//|     :param int ncols: number of columns to cast; limited to what the output
//|         buffers hold
//|     :param ~circuitpython_typing.ReadableBuffer wcolors: two ``uint16`` colors per
//|         wall type: near face at ``[type * 2]``, side face at ``[type * 2 + 1]``;
//|         at least types 0 and 1, and an unknown type uses type 1
//|     :param ~circuitpython_typing.WriteableBuffer top: at least ``ncols`` ``uint16``
//|         values; receives each column's wall top row
//|     :param ~circuitpython_typing.WriteableBuffer bot: at least ``ncols`` ``uint16``
//|         values; receives each column's wall bottom row
//|     :param ~circuitpython_typing.WriteableBuffer col: at least ``ncols`` ``uint16``
//|         values; receives each column's wall color
//|     :param ~circuitpython_typing.WriteableBuffer dist: at least ``ncols`` ``int32``
//|         values; receives each column's perpendicular distance (fixed-point)
//|     :param ~circuitpython_typing.WriteableBuffer runs: optional; at least
//|         ``5 * ncols`` ``uint16`` values, laid out as five ``ncols``-long planes
//|         (x0s, x1s, tops, bots, colors). When given, adjacent equal columns are
//|         merged into wall runs suitable for :py:meth:`picogame.Canvas.vspans` and the
//|         run count is returned; otherwise `None` is returned."""
//|     ...
//|
// C DDA wall raycaster for picogame_ray.Raycaster - INTEGER ONLY (16.16 fixed-point, no FPU; the paint,
// temporal invalidate, pose-cache and billboard math stay in Python; Python does the once-per-frame
// trig and passes Q16 ray params). map: read-only bytes, mw*mh wall types (0 = empty). pos*, l*x/l*y
// (leftRay, column 0), s*x/s*y (rayStep per column) are all 16.16. wcolors: uint16[(maxtype+1)*2] -
// [t*2] near, [t*2+1] side colour. top/bot/col: uint16 write buffers (len>=ncols); dist: int32 write
// buffer (perpendicular distance, 16.16). All arithmetic is 32-bit.
// Optional arg 17 (runs - ONE uint16 write buffer, len>=5*ncols, laid out as five ncols-long
// planes [x0s | x1s | tops | bots | colors]): also emit the RLE-MERGED wall runs (adjacent equal
// columns fused; x in PIXELS = column*stride) and return the run count. The planes feed
// Canvas.vspans directly as memoryview slices. This hoists picogame_ray's per-frame Python merge
// loop into the same C pass (measured 2-6.5 ms/frame of interpreted merge at stride=1 on RP2040).
// Callers clamp the LAST run's x1 to the screen width (stride rounding can overshoot by <stride).
// Without it: returns None.

// floor(2^32 / v) with one 32-bit divide. v must be >= 1.
static inline uint32_t pg_recip32(uint32_t v) {
    return (uint32_t)(0u - v) / v + 1u;
}

static mp_obj_t picogame3d_raycast(size_t n_args, const mp_obj_t *args) {
    mp_buffer_info_t mi, wi, ti, bi, ci, di;
    mp_get_buffer_raise(args[0], &mi, MP_BUFFER_READ);
    // Non-negative: the DDA below bounds-checks with unsigned compares.
    int mw = picogame_imax(mp_obj_get_int(args[1]), 0);
    // Unsigned: cells past mw * mh or past the map buffer read as wall.
    unsigned cells = (unsigned)picogame_imax(mp_obj_get_int(args[2]), 0) * (unsigned)mw;
    int32_t posx = mp_obj_get_int(args[3]);        // camera x, 16.16
    int32_t posy = mp_obj_get_int(args[4]);
    int32_t rdx = mp_obj_get_int(args[5]);         // leftRay x (column 0), 16.16 - accumulates per column
    int32_t rdy = mp_obj_get_int(args[6]);
    int32_t srx = mp_obj_get_int(args[7]);         // rayStep x per column, 16.16
    int32_t sry = mp_obj_get_int(args[8]);
    int sh = mp_obj_get_int(args[9]);
    int stride = mp_obj_get_int(args[10]);
    int ncols = mp_obj_get_int(args[11]);
    mp_get_buffer_raise(args[12], &wi, MP_BUFFER_READ);
    mp_get_buffer_raise(args[13], &ti, MP_BUFFER_WRITE);
    mp_get_buffer_raise(args[14], &bi, MP_BUFFER_WRITE);
    mp_get_buffer_raise(args[15], &ci, MP_BUFFER_WRITE);
    mp_get_buffer_raise(args[16], &di, MP_BUFFER_WRITE);
    uint16_t *r0 = NULL, *r1 = NULL, *rt = NULL, *rb = NULL, *rcol = NULL;
    if (n_args >= 18) {                            // run outputs requested
        mp_buffer_info_t q;
        mp_get_buffer_raise(args[17], &q, MP_BUFFER_WRITE);
        int cap = (int)(q.len / 10);               // five uint16 planes
        if (ncols > cap) {
            ncols = cap;                           // never write past the run planes
        }
        r0 = q.buf;
        r1 = r0 + cap;
        rt = r1 + cap;
        rb = rt + cap;
        rcol = rb + cap;
    }
    const uint8_t *map = mi.buf;
    const uint16_t *wc = wi.buf;
    // An unknown wall type falls back to type 1, so wcolors must hold types 0 and 1.
    int wc_types = (int)(mp_arg_validate_length_min(wi.len, 2 * 4, MP_QSTR_wcolors) >> 2);
    if (cells > mi.len) {
        cells = mi.len;
    }
    uint16_t *top = ti.buf;
    uint16_t *bot = bi.buf;
    uint16_t *col = ci.buf;
    int32_t *dist_out = di.buf;                    // perpendicular distance, 16.16
    int half = sh >> 1;
    int imapx0 = posx >> 16;
    int imapy0 = posy >> 16;
    int off0 = imapy0 * mw + imapx0;               // every column starts from the same cell
    int32_t fracx = posx & 0xFFFF;                 // fractional part of pos, 16.16
    int32_t fracy = posy & 0xFFFF;
    const int32_t DD_CAP = (int32_t)1 << 24;       // cap deltaDist so a 64-step accumulation stays in int32
    ncols = picogame_imin(ncols, (int)(picogame_imin(picogame_imin(ti.len, bi.len), ci.len) >> 1));
    ncols = picogame_imin(ncols, (int)(di.len >> 2));
    for (int c = 0; c < ncols; c++) {
        int mapx = imapx0;
        int mapy = imapy0;
        int32_t ax = rdx < 0 ? -rdx : rdx;
        int32_t ay = rdy < 0 ? -rdy : rdy;
        // deltaDist = 2^32 / |rayDir|, capped at DD_CAP. ax < 256 is exactly the capped
        // range, so the test also guards against ax == 0.
        int32_t ddx = (ax < 256) ? DD_CAP : (int32_t)pg_recip32((uint32_t)ax);
        int32_t ddy = (ay < 256) ? DD_CAP : (int32_t)pg_recip32((uint32_t)ay);
        int stepx, stepy;
        int32_t sidex, sidey;
        // sideDist to the first grid line = (fractional distance) * deltaDist, 16.16 (setup only)
        if (rdx < 0) {
            stepx = -1;
            sidex = pg_mulshr16((uint32_t)fracx, (uint32_t)ddx);
        } else {
            stepx = 1;
            sidex = pg_mulshr16((uint32_t)(65536 - fracx), (uint32_t)ddx);
        }
        if (rdy < 0) {
            stepy = -1;
            sidey = pg_mulshr16((uint32_t)fracy, (uint32_t)ddy);
        } else {
            stepy = 1;
            sidey = pg_mulshr16((uint32_t)(65536 - fracy), (uint32_t)ddy);
        }
        int side = 0;
        int cell = 1;
        // Linear map offset, stepped with the cell coordinates.
        int off = off0;
        int stepy_off = stepy * mw;
        for (int i = 0; i < 64; i++) {             // DDA - pure 32-bit
            if (sidex < sidey) {
                sidex += ddx;
                mapx += stepx;
                off += stepx;
                side = 0;
            } else {
                sidey += ddy;
                mapy += stepy;
                off += stepy_off;
                side = 1;
            }
            // Unsigned compares also reject negative coordinates.
            cell = ((unsigned)mapx < (unsigned)mw && (unsigned)off < cells)
                ? map[off] : 1;
            if (cell) {
                break;
            }
        }
        int32_t perp = (side == 0) ? (sidex - ddx) : (sidey - ddy);   // perpWallDist, 16.16
        if (perp < 655) {
            perp = 655;                            // ~0.01 in 16.16
        }
        int lh = (int)(((int32_t)sh << 16) / perp);   // sh / perpWallDist (px); 32-bit (sh<<16 <= ~15.7M)
        int t = half - (lh >> 1);
        int b = t + lh;
        if (t < 0) {
            t = 0;
        }
        if (b > sh) {
            b = sh;
        }
        top[c] = (uint16_t)t;
        bot[c] = (uint16_t)b;
        int ct = (cell < wc_types) ? cell : 1;     // unknown type -> type 1 (matches Python default)
        col[c] = wc[ct * 2 + side];
        dist_out[c] = perp;
        rdx += srx;                                // accumulate ray direction for the next column (no overflow)
        rdy += sry;
    }
    if (r0 && ncols > 0) {
        // post-pass RLE over the just-written (cache-hot) column arrays: one flush point
        int nr = 0;
        int rstart = 0;
        for (int c = 1; c <= ncols; c++) {
            if (c == ncols || top[c] != top[rstart] || bot[c] != bot[rstart] || col[c] != col[rstart]) {
                r0[nr] = (uint16_t)(rstart * stride);
                r1[nr] = (uint16_t)(c * stride);
                rt[nr] = top[rstart];
                rb[nr] = bot[rstart];
                rcol[nr] = col[rstart];
                nr++;
                rstart = c;
            }
        }
        return MP_OBJ_NEW_SMALL_INT(nr);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picogame3d_raycast_obj, 17, 18, picogame3d_raycast);

//| def road_edges(
//|     rl: WriteableBuffer,
//|     rr: WriteableBuffer,
//|     hw: ReadableBuffer,
//|     n: int,
//|     cx0: int,
//|     dist: int,
//|     cfg: ReadableBuffer,
//| ) -> None:
//|     """Compute one racing-road frame's left and right edge columns. A low-level
//|     interface used by the ``picogame_road`` helper, which packs these inputs.
//|
//|     Walks ``n`` screen rows bottom-up, accumulating the road curve, and writes the
//|     edge x coordinates into ``rl`` and ``rr``.
//|
//|     :param ~circuitpython_typing.WriteableBuffer rl: at least ``n`` ``int16`` values;
//|         receives the left edge per row
//|     :param ~circuitpython_typing.WriteableBuffer rr: at least ``n`` ``int16`` values;
//|         receives the right edge per row
//|     :param ~circuitpython_typing.ReadableBuffer hw: at least ``n`` ``int32`` per-row
//|         half-widths (signed 16.16 fixed-point)
//|     :param int n: number of rows to compute
//|     :param int cx0: screen center x including lateral offset (signed 16.16 fixed-point)
//|     :param int dist: integer world distance
//|     :param ~circuitpython_typing.ReadableBuffer cfg: seven ``int32`` curve parameters,
//|         in order: ``f1_q20``, ``f2_q20``, ``amp1k_q16``, ``amp2k_q16``, ``world_step``,
//|         ``curve_step``, ``d_row_off``"""
//|     ...
//|
// road_edges(rl, rr, hw, n, cx0, dist, cfg) - one racing-road frame's curve accumulator + integer
// edges in one call (the OutRun-genre compute_road loop; core + cfg layout documented in
// shared-module). rl/rr = int16 out, hw = int32 Q16 half-widths, cx0 = Q16 screen centre
// (incl. lateral), dist = integer world distance, cfg = int32[7].
static mp_obj_t picogame3d_road_edges_fn(size_t n_args, const mp_obj_t *args) {
    mp_buffer_info_t rli, rri, hwi, cfgi;
    mp_get_buffer_raise(args[0], &rli, MP_BUFFER_WRITE);
    mp_get_buffer_raise(args[1], &rri, MP_BUFFER_WRITE);
    mp_get_buffer_raise(args[2], &hwi, MP_BUFFER_READ);
    mp_get_buffer_raise(args[6], &cfgi, MP_BUFFER_READ);
    int n = mp_obj_get_int(args[3]);
    int cap = (int)(rli.len < rri.len ? rli.len : rri.len) / 2;
    if (n > cap) {
        n = cap;
    }
    if (n > (int)(hwi.len / 4)) {
        n = (int)(hwi.len / 4);
    }
    if (n <= 0 || cfgi.len < 7 * 4) {
        return mp_const_none;
    }
    picogame3d_road_edges((int16_t *)rli.buf, (int16_t *)rri.buf, (const int32_t *)hwi.buf, n,
        mp_obj_get_int(args[4]), mp_obj_get_int(args[5]), (const int32_t *)cfgi.buf);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(picogame3d_road_edges_obj, 7, 7, picogame3d_road_edges_fn);

static const mp_rom_map_elem_t picogame3d_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_picogame3d) },
    { MP_ROM_QSTR(MP_QSTR_mode7), MP_ROM_PTR(&picogame3d_mode7_obj) },
    { MP_ROM_QSTR(MP_QSTR_road), MP_ROM_PTR(&picogame3d_road_obj) },
    { MP_ROM_QSTR(MP_QSTR_road_edges), MP_ROM_PTR(&picogame3d_road_edges_obj) },
    { MP_ROM_QSTR(MP_QSTR_raycast), MP_ROM_PTR(&picogame3d_raycast_obj) },
    { MP_ROM_QSTR(MP_QSTR_project), MP_ROM_PTR(&picogame3d_project_obj) },
    { MP_ROM_QSTR(MP_QSTR_project_mesh), MP_ROM_PTR(&picogame3d_project_mesh_obj) },
    { MP_ROM_QSTR(MP_QSTR_FLOAT), MP_ROM_INT(CIRCUITPY_PICOGAME3D_FLOAT) },
};
static MP_DEFINE_CONST_DICT(picogame3d_module_globals, picogame3d_module_globals_table);

const mp_obj_module_t picogame3d_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&picogame3d_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_picogame3d, picogame3d_module);
