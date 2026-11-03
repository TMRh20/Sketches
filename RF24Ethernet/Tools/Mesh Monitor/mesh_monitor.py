#!/usr/bin/env python3
"""Live radial view of an RF24Mesh dhcplist.txt.

The file is RF24Mesh's binary address list: repeated 4-byte entries
  uint8 nodeID, uint8 padding, uint16 address (little endian)
Addresses are RF24Network octal addresses; each octal digit is one hop,
so the number of octal digits is the distance from the master (00).
Address 0 means the entry has been released.

Usage: python3 mesh_monitor.py [--file ~/dhcplist.txt] [--port 8080] [--host 0.0.0.0]
Then open http://<pi-ip>:8080/
"""
import argparse
import json
import os
import struct
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

ARGS = None


def read_list(path):
    with open(path, "rb") as f:
        data = f.read()
    nodes = []
    for i in range(0, len(data) - len(data) % 4, 4):
        node_id, _pad, addr = struct.unpack_from("<BBH", data, i)
        nodes.append({"id": node_id, "addr": addr})
    st = os.stat(path)
    return {"nodes": nodes, "mtime": st.st_mtime, "size": len(data)}


PAGE = r"""<!doctype html>
<html><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Mesh Monitor</title>
<style>
:root{--bg:#0f1115;--panel:#171a21;--fg:#e6e8ee;--mute:#8b92a3;--ring:#2a2f3a;--edge:#4a5263;
--d0:#e6e8ee;--d1:#3ddc5a;--d2:#f5d742;--d3:#3fd8e6;--d4:#ff6fae;--d5:#ff4d4d;--warn:#ff5555}
@media (prefers-color-scheme: light){:root{--bg:#f6f7f9;--panel:#fff;--fg:#1b1f27;--mute:#5d6576;
--ring:#dde1e8;--edge:#a7aebb;--d0:#5d6576;--d1:#1e9e3a;--d2:#b89400;--d3:#0f9aaa;--d4:#d6337f;--d5:#c62828}}
*{box-sizing:border-box}body{margin:0;background:var(--bg);color:var(--fg);font:14px system-ui,sans-serif}
header{display:flex;flex-wrap:wrap;gap:12px 24px;align-items:baseline;padding:14px 16px}
h1{font-size:18px;margin:0}.mute{color:var(--mute)}
main{display:flex;flex-wrap:wrap;gap:16px;padding:0 16px 16px}
#chartWrap{flex:1 1 560px;min-width:0;background:var(--panel);border-radius:10px;padding:8px}
svg{width:100%;height:auto;display:block}
aside{flex:0 1 300px;min-width:0;background:var(--panel);border-radius:10px;padding:12px 14px}
aside h2{font-size:14px;margin:10px 0 6px}
table{border-collapse:collapse;width:100%;font-variant-numeric:tabular-nums}
td,th{padding:3px 6px;text-align:left}th{color:var(--mute);font-weight:500}
.chip{display:inline-block;width:10px;height:10px;border-radius:50%;margin-right:6px;vertical-align:-1px}
.released{font-family:ui-monospace,monospace;font-size:12px;color:var(--mute);word-break:break-word}
.node text{font-size:11px;fill:var(--fg);pointer-events:none}
.node circle{stroke:var(--panel);stroke-width:2}
.ghost circle{fill:none!important;stroke:var(--edge);stroke-dasharray:3 3}
.dup circle{stroke:var(--warn);stroke-width:3}
.ringlbl{font-size:11px;fill:var(--mute)}
.flash{animation:fl 1.2s}@keyframes fl{0%{opacity:.2}100%{opacity:1}}
</style></head><body>
<header><h1>RF24Mesh: node distance from master</h1>
<span id="status" class="mute">loading…</span></header>
<main><div id="chartWrap"><svg id="svg" viewBox="-400 -400 800 800"></svg></div>
<aside><h2>Nodes per hop</h2><table id="hops"></table>
<h2>Active nodes</h2><table id="list"></table>
<h2>Released IDs (address 0)</h2><div id="rel" class="released"></div></aside></main>
<script>
const NS="http://www.w3.org/2000/svg";
const depthOf=a=>{let d=0;while(a){d++;a>>=3}return d};
const parentOf=a=>{const d=depthOf(a);return d<=1?0:a&((1<<(3*(d-1)))-1)};
const oct=a=>"0"+a.toString(8);
const color=d=>`var(--d${Math.min(d,5)})`;
let lastSig="";
function el(t,attrs,parent){const e=document.createElementNS(NS,t);for(const k in attrs)e.setAttribute(k,attrs[k]);parent&&parent.appendChild(e);return e}

function render(data){
  const active=data.nodes.filter(n=>n.addr!==0);
  const released=data.nodes.filter(n=>n.addr===0).map(n=>n.id);
  // build tree keyed by address, inserting placeholder parents if missing
  const T=new Map();T.set(0,{addr:0,ids:["master"],kids:[],ghost:false});
  const ensure=a=>{if(T.has(a))return T.get(a);const n={addr:a,ids:[],kids:[],ghost:true};T.set(a,n);
    ensure(parentOf(a)).kids.push(n);return n};
  for(const n of active){const t=ensure(n.addr);t.ids.push(n.id);t.ghost=false}
  for(const t of T.values())t.kids.sort((x,y)=>x.addr-y.addr);
  const maxD=Math.max(1,...[...T.keys()].map(depthOf));
  const R=340,step=R/maxD;
  // angular span proportional to leaf count
  const leaves=t=>t._l=t.kids.length?t.kids.reduce((s,k)=>s+leaves(k),0):1;leaves(T.get(0));
  const place=(t,a0,a1)=>{t.ang=(a0+a1)/2;t.r=depthOf(t.addr)*step;let a=a0;
    for(const k of t.kids){const w=(a1-a0)*k._l/t._l;place(k,a,a+w);a+=w}};
  place(T.get(0),-Math.PI/2,Math.PI*1.5);
  const xy=t=>[t.r*Math.cos(t.ang),t.r*Math.sin(t.ang)];

  const svg=document.getElementById("svg");svg.innerHTML="";svg.classList.remove("flash");void svg.offsetWidth;svg.classList.add("flash");
  for(let d=1;d<=maxD;d++){el("circle",{r:d*step,fill:"none",stroke:"var(--ring)"},svg);
    el("text",{x:4,y:-d*step+12,class:"ringlbl"},svg).textContent=`${d} hop${d>1?"s":""}`}
  for(const t of T.values()){if(t.addr===0)continue;const[x,y]=xy(t),[px,py]=xy(T.get(parentOf(t.addr)));
    el("line",{x1:px,y1:py,x2:x,y2:y,stroke:"var(--edge)","stroke-width":1.5},svg)}
  for(const t of T.values()){const d=depthOf(t.addr),[x,y]=xy(t);
    const g=el("g",{class:"node"+(t.ghost?" ghost":"")+(t.ids.length>1&&t.addr?" dup":""),transform:`translate(${x},${y})`},svg);
    el("circle",{r:d===0?16:10,fill:color(d)},g);
    const ttl=el("title",{},g);ttl.textContent=t.addr===0?"Master (00)":t.ghost?`${oct(t.addr)}: no node assigned (parent placeholder)`
      :`Node ${t.ids.join(", ")} at ${oct(t.addr)}, ${d} hop${d>1?"s":""}`+(t.ids.length>1?" (DUPLICATE ADDRESS)":"");
    const lbl=t.addr===0?"M":t.ghost?"":t.ids.join(",");
    const out=d>0, ox=Math.cos(t.ang)*(out?18:0), oy=Math.sin(t.ang)*(out?18:0)+4;
    el("text",{x:ox,y:oy,"text-anchor":out?(Math.cos(t.ang)>.3?"start":Math.cos(t.ang)<-.3?"end":"middle"):"middle"},g).textContent=lbl;
    if(!t.ghost&&d>0){el("text",{x:ox,y:oy+12,"text-anchor":out?(Math.cos(t.ang)>.3?"start":Math.cos(t.ang)<-.3?"end":"middle"):"middle",
      style:"fill:var(--mute);font-size:10px"},g).textContent=oct(t.addr)}
  }
  // side tables
  const counts={};for(const n of active){const d=depthOf(n.addr);counts[d]=(counts[d]||0)+1}
  document.getElementById("hops").innerHTML="<tr><th>Hops</th><th>Nodes</th></tr>"+
    Object.keys(counts).sort().map(d=>`<tr><td><span class="chip" style="background:${color(+d)}"></span>${d}</td><td>${counts[d]}</td></tr>`).join("");
  const dupAddr=new Set(active.map(n=>n.addr).filter((a,i,arr)=>arr.indexOf(a)!==i));
  document.getElementById("list").innerHTML="<tr><th>ID</th><th>Address</th><th>Hops</th></tr>"+
    active.slice().sort((a,b)=>depthOf(a.addr)-depthOf(b.addr)||a.addr-b.addr).map(n=>
    `<tr${dupAddr.has(n.addr)?' style="color:var(--warn)"':""}><td>${n.id}</td><td>${oct(n.addr)}</td><td>${depthOf(n.addr)}</td></tr>`).join("");
  document.getElementById("rel").textContent=released.length?released.sort((a,b)=>a-b).join(", "):"none";
  return {active:active.length,released:released.length};
}

async function poll(){
  try{const r=await fetch("data.json",{cache:"no-store"});const d=await r.json();
    const sig=JSON.stringify(d.nodes);let info;
    if(sig!==lastSig){lastSig=sig;info=render(d);window._info=info}
    info=window._info;
    document.getElementById("status").textContent=`${info.active} active, ${info.released} released · file updated ${new Date(d.mtime*1000).toLocaleTimeString()} · checked ${new Date().toLocaleTimeString()}`;
  }catch(e){document.getElementById("status").textContent="error: "+e}
}
poll();setInterval(poll,2000);
</script></body></html>"""


class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path.startswith("/data.json"):
            try:
                body = json.dumps(read_list(ARGS.file)).encode()
                code = 200
            except OSError as e:
                body = json.dumps({"error": str(e), "nodes": [], "mtime": 0}).encode()
                code = 500
            ctype = "application/json"
        elif self.path in ("/", "/index.html"):
            body, code, ctype = PAGE.encode(), 200, "text/html; charset=utf-8"
        else:
            body, code, ctype = b"not found", 404, "text/plain"
        self.send_response(code)
        self.send_header("Content-Type", ctype)
        self.send_header("Cache-Control", "no-store")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def log_message(self, *a):
        pass


def main():
    global ARGS
    p = argparse.ArgumentParser()
    p.add_argument("--file", default=os.path.expanduser("~/dhcplist.txt"))
    p.add_argument("--port", type=int, default=8080)
    p.add_argument("--host", default="0.0.0.0")
    ARGS = p.parse_args()
    print(f"Watching {ARGS.file} - open http://{ARGS.host}:{ARGS.port}/")
    ThreadingHTTPServer((ARGS.host, ARGS.port), Handler).serve_forever()


if __name__ == "__main__":
    main()
