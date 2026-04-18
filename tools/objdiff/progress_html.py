#!/usr/bin/env python3
"""Generate an HTML progress report from objdiff's report.json."""

import json
import sys
from pathlib import Path


def generate_html(report, out_path):
    m = report["measures"]
    units = report["units"]
    categories = report.get("categories", [])

    # Build category -> units mapping from metadata
    cat_units = {}
    for u in units:
        for cid in u.get("metadata", {}).get("progress_categories", []):
            cat_units.setdefault(cid, []).append(u)

    # Sort units within each category by match percent descending
    for unit_list in cat_units.values():
        unit_list.sort(key=lambda u: -float(u["measures"].get("matched_code_percent", 0)))

    # Sort categories by match percent descending
    categories = sorted(categories, key=lambda c: -float(c["measures"].get("matched_code_percent", 0)))

    total_funcs = m["total_functions"]
    matched_funcs = m["matched_functions"]
    func_pct = m["matched_functions_percent"]
    code_pct = m["matched_code_percent"]
    total_code = int(m["total_code"])
    matched_code = int(m["matched_code"])

    html = f"""<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>FF8 Decomp Progress</title>
<style>
  :root {{
    --bg: #0d1117; --fg: #c9d1d9; --border: #30363d;
    --bar-bg: #21262d; --bar-fill: #238636; --bar-partial: #d29922;
    --link: #58a6ff; --header-bg: #161b22;
  }}
  * {{ box-sizing: border-box; margin: 0; padding: 0; }}
  body {{ font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Helvetica, Arial, sans-serif;
         background: var(--bg); color: var(--fg); padding: 24px; max-width: 1100px; margin: 0 auto; }}
  h1 {{ margin-bottom: 8px; font-size: 24px; }}
  .subtitle {{ color: #8b949e; margin-bottom: 24px; font-size: 14px; }}
  .summary {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
              gap: 16px; margin-bottom: 32px; }}
  .stat-card {{ background: var(--header-bg); border: 1px solid var(--border);
                border-radius: 8px; padding: 16px; }}
  .stat-card .label {{ color: #8b949e; font-size: 12px; text-transform: uppercase; letter-spacing: 0.5px; }}
  .stat-card .value {{ font-size: 28px; font-weight: 600; margin: 4px 0; }}
  .stat-card .detail {{ color: #8b949e; font-size: 13px; }}
  .bar-outer {{ background: var(--bar-bg); border-radius: 4px; height: 8px; margin-top: 8px; overflow: hidden; }}
  .bar-inner {{ height: 100%; border-radius: 4px; transition: width 0.3s; }}
  .bar-green {{ background: var(--bar-fill); }}
  .bar-yellow {{ background: var(--bar-partial); }}
  .category {{ margin-bottom: 24px; }}
  .category-header {{ display: flex; justify-content: space-between; align-items: center;
                      padding: 12px 16px; background: var(--header-bg); border: 1px solid var(--border);
                      border-radius: 8px 8px 0 0; cursor: pointer; user-select: none; }}
  .category-header:hover {{ background: #1c2128; }}
  .category-header h3 {{ font-size: 15px; font-weight: 600; }}
  .category-header .pct {{ font-size: 14px; color: #8b949e; }}
  .category-body {{ border: 1px solid var(--border); border-top: none; border-radius: 0 0 8px 8px; }}
  .unit-row {{ display: grid; grid-template-columns: 1fr 100px 200px 60px;
               align-items: center; padding: 8px 16px; border-bottom: 1px solid var(--border);
               font-size: 13px; }}
  .unit-row:last-child {{ border-bottom: none; }}
  .unit-row:hover {{ background: #161b22; }}
  .unit-name {{ font-family: 'SFMono-Regular', Consolas, monospace; font-size: 12px; }}
  .unit-funcs {{ text-align: right; color: #8b949e; }}
  .unit-bar {{ padding: 0 8px; }}
  .unit-bar .bar-outer {{ height: 6px; }}
  .unit-pct {{ text-align: right; font-weight: 500; font-size: 12px; }}
  .toggle {{ font-size: 12px; color: #8b949e; }}
  details summary {{ list-style: none; }}
  details summary::-webkit-details-marker {{ display: none; }}
  details[open] .toggle {{ transform: rotate(90deg); display: inline-block; }}
  .pct-100 {{ color: var(--bar-fill); }}
  .pct-high {{ color: #58a6ff; }}
  .pct-mid {{ color: var(--bar-partial); }}
  .pct-low {{ color: #8b949e; }}
</style>
</head>
<body>
<h1>FF8 Decomp Progress</h1>
<p class="subtitle">Final Fantasy VIII (PS1, USA) &mdash; SLUS_008.92</p>

<div class="summary">
  <div class="stat-card">
    <div class="label">Functions</div>
    <div class="value">{matched_funcs:,} <span style="font-size:16px;color:#8b949e">/ {total_funcs:,}</span></div>
    <div class="detail">{func_pct:.1f}% matched</div>
    <div class="bar-outer"><div class="bar-inner bar-green" style="width:{func_pct:.1f}%"></div></div>
  </div>
  <div class="stat-card">
    <div class="label">Code Bytes</div>
    <div class="value">{matched_code/1024:.1f} <span style="font-size:16px;color:#8b949e">/ {total_code/1024:.1f} KB</span></div>
    <div class="detail">{code_pct:.1f}% matched</div>
    <div class="bar-outer"><div class="bar-inner bar-green" style="width:{code_pct:.1f}%"></div></div>
  </div>
  <div class="stat-card">
    <div class="label">Compilation Units</div>
    <div class="value">{len(units)}</div>
    <div class="detail">across {len(categories)} binaries</div>
  </div>
</div>

"""

    for cat in categories:
        cm = cat["measures"]
        cid = cat["id"]
        cname = cat["name"]
        cat_funcs = cm.get("total_functions", 0)
        cat_matched = cm.get("matched_functions", 0)
        cat_total_code = int(cm.get("total_code", "0"))
        cat_matched_code = int(cm.get("matched_code", "0"))
        pct = float(cm.get("matched_code_percent", 0))

        if cat_funcs == 0:
            continue

        pct_class = "pct-100" if pct == 100 else "pct-high" if pct >= 50 else "pct-mid" if pct >= 10 else "pct-low"

        html += f"""<div class="category">
<details {"open" if pct > 0 else ""}>
<summary>
<div class="category-header">
  <h3>{cname}</h3>
  <div>
    <span class="pct">{cat_matched}/{cat_funcs} funcs &nbsp; {cat_matched_code/1024:.0f}/{cat_total_code/1024:.0f} KB</span>
    &nbsp; <span class="{pct_class}">{pct:.1f}%</span>
    &nbsp; <span class="toggle">&#9656;</span>
  </div>
</div>
</summary>
<div class="category-body">
"""
        for u in cat_units.get(cid, []):
            um = u["measures"]
            mf = um.get("matched_functions", 0)
            tf = um.get("total_functions", 0)
            up = float(um.get("matched_code_percent", 0))
            if tf == 0:
                continue
            up_class = "pct-100" if up == 100 else "pct-high" if up >= 50 else "pct-mid" if up >= 10 else "pct-low"
            ubar = "bar-green" if up >= 50 else "bar-yellow"

            html += f"""  <div class="unit-row">
    <span class="unit-name">{u['name']}</span>
    <span class="unit-funcs">{mf}/{tf}</span>
    <span class="unit-bar"><div class="bar-outer"><div class="bar-inner {ubar}" style="width:{up:.1f}%"></div></div></span>
    <span class="unit-pct {up_class}">{up:.1f}%</span>
  </div>
"""

        html += """</div>
</details>
</div>
"""

    html += f"""
<p class="subtitle" style="margin-top:32px; text-align:center;">
  Generated by objdiff &mdash; <a href="https://github.com/encounter/objdiff" style="color:var(--link)">github.com/encounter/objdiff</a>
</p>
</body>
</html>"""

    with open(out_path, "w") as f:
        f.write(html)

    print(f"Generated {out_path}")


def main():
    json_path = sys.argv[1] if len(sys.argv) > 1 else "build/report.json"
    out_path = sys.argv[2] if len(sys.argv) > 2 else "build/progress.html"

    with open(json_path) as f:
        report = json.load(f)

    generate_html(report, out_path)


if __name__ == "__main__":
    main()
