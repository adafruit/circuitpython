// Shows the device's display. The protocol is described in
// supervisor/shared/web_workflow/display_stream.h.
// Wrapped so its names don't clash with serial.js on the same page.
(function() {
  var canvas = document.getElementById("screen");
  var context = canvas.getContext("2d");
  var view = document.getElementById("view");
  var status_text = document.getElementById("status");
  var ws;
  // Set when another page took the display over, so this one doesn't take it back.
  var replaced = false;

  var DEPTHS = {
    "bgrx8888": 32, "rgb565le": 16, "rgb565be": 16, "rgb332": 8, "gray8": 8,
    "rgbd4": 4, "gray4": 4, "gray2": 2, "gray1": 1
  };

  var info = null;
  var depth = 0;
  var unit = 1;
  var image = null;
  var pixels = null;
  // Framebuffer value to canvas pixel (ABGR, which is RGBA in little endian memory).
  var colors = null;
  var updates = 0;
  var received = 0;
  // Degrees clockwise the picture is turned. Only the page turns it, the device
  // sends the display as it is. Remembered by the browser where it can.
  var rotation = 0;
  try {
    rotation = (parseInt(localStorage.getItem("display_rotation")) || 0) % 360;
  } catch (e) {
  }

  // Fits the turned picture to the width of its column and three quarters of
  // the window's height.
  function layout() {
    var turned = rotation % 180 != 0;
    var shown_width = turned ? canvas.height : canvas.width;
    var shown_height = turned ? canvas.width : canvas.height;
    var scale = Math.min(view.clientWidth / shown_width, window.innerHeight * 0.75 / shown_height);
    view.style.height = shown_height * scale + "px";
    canvas.style.width = canvas.width * scale + "px";
    canvas.style.height = canvas.height * scale + "px";
    canvas.style.left = (view.clientWidth - canvas.width * scale) / 2 + "px";
    canvas.style.top = (shown_height - canvas.height) * scale / 2 + "px";
    canvas.style.transform = "rotate(" + rotation + "deg)";
  }

  function rgba(r, g, b) {
    return (0xff000000 | (b << 16) | (g << 8) | r) >>> 0;
  }

  function setup(new_info) {
    info = new_info;
    depth = DEPTHS[info.format] || 0;
    if (depth == 0) {
      image = null;
      status_text.textContent = "No display";
      return;
    }
    unit = depth >= 8 ? depth / 8 : 1;
    canvas.width = info.width;
    canvas.height = info.height;
    layout();
    image = context.createImageData(info.width, info.height);
    pixels = new Uint32Array(image.data.buffer);
    colors = null;
    if (info.format.startsWith("rgb565")) {
      colors = new Uint32Array(65536);
      for (var v = 0; v < 65536; v++) {
        var r = v >> 11, g = (v >> 5) & 0x3f, b = v & 0x1f;
        colors[v] = rgba((r << 3) | (r >> 2), (g << 2) | (g >> 4), (b << 3) | (b >> 2));
      }
    } else if (info.format == "rgb332") {
      colors = new Uint32Array(256);
      for (var v = 0; v < 256; v++) {
        var r = v >> 5, g = (v >> 2) & 7, b = v & 3;
        colors[v] = rgba(r * 255 / 7, g * 255 / 7, b * 255 / 3);
      }
    } else if (info.format == "rgbd4") {
      colors = new Uint32Array(16);
      for (var v = 0; v < 16; v++) {
        colors[v] = rgba((v >> 3) & 1 ? 255 : 0, (v >> 2) & 1 ? 255 : 0, (v >> 1) & 1 ? 255 : 0);
      }
    } else if (info.format.startsWith("gray")) {
      var levels = 1 << depth;
      colors = new Uint32Array(levels);
      for (var v = 0; v < levels; v++) {
        var l = Math.round(v * 255 / (levels - 1));
        colors[v] = rgba(l, l, l);
      }
    }
  }

  // Undoes the run length encoding into out.
  function decode(data, offset, out) {
    var o = 0;
    while (offset < data.length && o < out.length) {
      var c = data[offset++];
      if (c < 128) {
        var n = (c + 1) * unit;
        out.set(data.subarray(offset, offset + n), o);
        offset += n;
        o += n;
      } else {
        var count = c - 126;
        for (var i = 0; i < count; i++) {
          for (var j = 0; j < unit; j++) {
            out[o++] = data[offset + j];
          }
        }
        offset += unit;
      }
    }
  }

  function draw(data) {
    if (image == null || data[0] != 1) {
      return;
    }
    var x = data[2] | (data[3] << 8);
    var y = data[4] | (data[5] << 8);
    var w = data[6] | (data[7] << 8);
    var h = data[8] | (data[9] << 8);
    var row_bytes = Math.ceil(w * depth / 8);
    var rows;
    if (data[1] == 1) {
      rows = new Uint8Array(row_bytes * h);
      decode(data, 10, rows);
    } else {
      rows = data.subarray(10);
    }
    var per_byte = 8 / depth;
    var mask = (1 << depth) - 1;
    for (var j = 0; j < h; j++) {
      var src = j * row_bytes;
      var dst = (y + j) * info.width + x;
      for (var i = 0; i < w; i++) {
        var p;
        if (depth == 32) {
          var s = src + i * 4;
          p = rgba(rows[s + 2], rows[s + 1], rows[s]);
        } else if (depth == 16) {
          var s = src + i * 2;
          var v = info.format == "rgb565le" ? rows[s] | (rows[s + 1] << 8) : (rows[s] << 8) | rows[s + 1];
          p = colors[v];
        } else if (depth == 8) {
          p = colors[rows[src + i]];
        } else {
          var byte = rows[src + Math.floor(i / per_byte)];
          var index = i % per_byte;
          var shift = info.lsb_first ? index * depth : 8 - depth - index * depth;
          p = colors[(byte >> shift) & mask];
        }
        pixels[dst + i] = p;
      }
    }
    context.putImageData(image, 0, 0, x, y, w, h);
    updates++;
  }

  function connect() {
    status_text.textContent = "Connecting";
    replaced = false;
    ws = new WebSocket("ws://" + window.location.host + "/cp/display/");
    ws.binaryType = "arraybuffer";
    ws.onmessage = function(e) {
      if (typeof e.data == "string") {
        setup(JSON.parse(e.data));
        return;
      }
      received += e.data.byteLength;
      draw(new Uint8Array(e.data));
    };
    ws.onclose = function(e) {
      if (e.code == 4000) {
        replaced = true;
        status_text.textContent = "Another page took over the display. Refresh to show it here.";
        return;
      }
      status_text.textContent = "Disconnected";
      setTimeout(connect, 2000);
    };
  }

  document.getElementById("refresh").onclick = function() {
    if (ws && ws.readyState == WebSocket.OPEN) {
      ws.send("f");
    } else if (replaced) {
      connect();
    }
  };

  function rotate(degrees) {
    rotation = degrees % 360;
    try {
      if (rotation == 0) {
        localStorage.removeItem("display_rotation");
      } else {
        localStorage.setItem("display_rotation", rotation);
      }
    } catch (e) {
    }
    layout();
  }

  document.getElementById("rotate").onclick = function() {
    rotate(rotation + 90);
  };
  // Back to the display as the device sends it.
  document.getElementById("unrotate").onclick = function() {
    rotate(0);
  };
  window.addEventListener("resize", layout);
  layout();

  setInterval(function() {
    if (ws && ws.readyState == WebSocket.OPEN && image != null) {
      status_text.textContent = info.width + "x" + info.height + " " + info.format + ", " +
        updates + " updates/s, " + (received / 1024).toFixed(1) + " KB/s";
    }
    updates = 0;
    received = 0;
  }, 1000);

  connect();
})();
