const char index_html[] PROGMEM = R"=====(<!-- Software develop by CRM/DEV (telegram: @User624) -->
<!DOCTYPE HTML>
<html lang="en">
  <head>
    <title id="pageNameExt">Bierlift</title>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="mobile-web-app-capable" content="yes">
    <meta name="apple-mobile-web-app-capable" content="yes">
    <meta name="msapplication-starturl" content="/">
    <link rel="apple-touch-icon" href="icon-192x192.png">
    <link rel="manifest" href="manifest.json">
    <link rel="stylesheet" href="notif.css">
    <link rel="stylesheet" href="style.css">
  </head>
  <body>
    <input type="checkbox" id="drawer-toggle"/>
    <label for="drawer-toggle" id="drawer-toggle-label"></label>
    <header id="pageName"></header>
    <div id="header-r"></div>
    <nav id="drawer">
      <div id="app-name" onclick="window.location.reload();"></div>
      <hr class="menu-hr">
      <table class="sys-t"><tbody><tr><td class="td1">
      <ul id="menu"></ul>
        </td></tr><tr><td class="td2">
        <div id="sys-info">
          <div title="Uptime"><span class="crm-icon sys-info-i">&#xe80b;</span><span class="sys-info-t" id="uptime">...</span></div>
          <div title="WiFi RSSI"><span class="crm-icon sys-info-i wf">&#xf1eb;</span><span class="sys-info-t" id="wifi">...</span></div>
          <div title="RAM"><span class="crm-icon sys-info-i">&#xf2db;</span><span class="sys-info-t" id="ram">...</span></div>
          <div title="ID"><span class="crm-icon sys-info-i">&#xf21b;</span><span class="sys-info-t" id="devid">...</span></div>
          <div title="IP address"><span class="crm-icon sys-info-i">IP</span><span class="sys-info-t" id="devip">...</span></div>
          <div title="Version"><span class="crm-icon sys-info-i">&#xf1c9;</span><span class="sys-info-t" id="fw">...</span></div>
        </div>
        <div id="sys-menu">
          <hr class="menu-hr">
          <div class="ke"><span>l</span><span>kessler engineering</span></div>
          <span class="crm-icon sys-menu-item" onclick="updatepage(1)">&#x1f4a6;</span>
          <span class="crm-icon sys-menu-item" title="kessler engineering | bierlift.de" onclick="window.open('http://bierlift.de','_blank');">&#xe4af;</span>
          <span class="crm-icon sys-menu-item" title="kessler-engineering@gmx.de" onclick="window.open('mailto:kessler-engineering@gmx.de','_blank');">&#xf0e0;</span>
          <span class="crm-icon sys-menu-item" title="FAQ" onclick="window.open('https://www.bierlift.de/faq/','_blank');">&#xe80e;</span>
        </div>
        </td></tr></tbody></table>
    </nav>
    <div id="page-content"></div>
  <script src="notif.js"></script>
  <script src="script.js"></script>
  </body>
</html>)=====";
