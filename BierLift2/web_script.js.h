// Browser-Skript der Weboberfläche (aus CRMui3).
// Seit 2026.11.15b ist die LESBARE Fassung aktiv. Bis 2026.11.15a wurde eine verschleierte Fassung
// ausgeliefert; sie wurde per Entschlüsselung mit dieser Fassung verglichen (Texte, Namen, Zahlen gleich).
// Übernommen wurden daraus: Vorgabewerte leerer Datums-/Zeitfelder (2021-12-02, 12:21) und die
// Wischgeste fürs Menü (400 ms, 50 px). Weggefallen ist nur der Schutzcode des Verschleierers
// (Konsole stummgeschaltet, Debugger-Falle alle 4 s).
// Texte der WLAN- und Systemseite: Array "lang" unten. Ausgeliefert wird die gzip-Fassung
// web_gz.h, die tools/web_gzip.py beim Build aus dieser Datei erzeugt.
const char script_js[] PROGMEM = R"=====(

let url = window.location.href.split('//')[1].split('/')[0],
    ui = null, page = 0, WFSS = false;
var _C1, _C2, socket, connected = false, changeTimer = null, chartArray = {};
var lang = JSON.parse(`{
  "ru":["Режим работы WiFi","Ожидать подключения","Имя сети","Пароль","Имя точки доступа","Точка доступа","Клиент"," сек", "Основной цвет", "Цвет значений", "Конфигурация","СБРОСИТЬ","СОХРАНИТЬ","ОБНОВИТЬ","Прошивка","Система","Соединение установлено","Соединение потеряно","Имя","Канал","Защита","Поиск WiFi, подождите.","Закрыть","Сети Wi-Fi не найдены."],
  "en":["WiFi mode","Max. wait for connection","WiFi name","Password","Access point name","Access point","Client"," sec","Main color","Value color","Configuration","RESET","SAVE","UPDATE","Firmware","System","Connection established","Connection lost","Name","Channel","Security","WiFi search, please wait.","Close","No Wi-Fi networks found."],
  "de":["WLAN Modus","Max. Wartezeit WLAN-Verbindung","WLAN Name","Passwort","Name des Access Points","Access point","Client"," sec","Hauptfarbe","Farbe der Werte","Konfiguration","ZURÜCKSETZEN","SPEICHERN","AKTUALISIEREN","Firmware","System","Verbindung hergestellt","Verbindung unterbrochen","Name","Kanal","Verschlüsselung","WLAN-Suche, bitte warten.","Schließen","Keine WLAN-Netze gefunden."]
  }`);


const doc = (id) => {
  return document.getElementById(id);
}

function exist (i) {
  return typeof i !== "undefined";
}

const getUI = (f = 1) => {
  let xhr = new XMLHttpRequest();
  xhr.open('POST', `http://${url}/ui`, true);
  xhr.onload = function() {
    try {
      ui = JSON.parse(xhr.responseText);
      document.documentElement.style.setProperty('--main-color', ui.cfg._C1);
      document.documentElement.style.setProperty('--value-color', ui.cfg._C2);
      wifi();
      if (f) uiUpd();
      menu();
      if (!connected) ws();
    } catch (e) {
      setTimeout(() => getUI(), 1000);
      console.log("Config error, repeated request after 1s.: ", e);
    }
  }
  xhr.send(null);
  
}


const ws = () => {
  socket = new WebSocket(`ws://${url}/ws`);
  socket.onmessage = function(event) {
    let data = JSON.parse(event.data);
    if (data._t == 0) getUI();
    if (data._t == 2) {
      for (let i = 0; i < data.d.length; i++) {
        set(data.d[i][0], data.d[i][1]);
      }
    } else if (data._t == 3) {
      notif(data.d[0], data.d[1], data.d[2], data.d[3]);
    }
  }
  
  socket.onopen = function() {
    connected = true;
    setTimeout(() => notif("i", lang[ui.cfg._L][16], 3, 0), 500);
  }
  
  socket.onclose = function(event) {
    if (!event.wasClean && connected) {
      notif("i", lang[ui.cfg._L][17], 3, 0);
      connected = false;
    }
    setTimeout(() => ws(), 1000);
  }
}


const icon = (s) => {
  let i = s.indexOf("&#x");
  while(i != -1) {
  if (s.substring(i -3, i) != "<z>") {
    let a = s.slice(0, i);
    a += `<z>${s.slice(i, i + 8)}</z>`;
    s = a += s.slice(i + 8);
  }
  i = s.indexOf("&#x", i + 8);
  }
  return s;
}


const noIcon = (s) => {
  let i = s.indexOf("&#");
  while(i != -1) {
  if (s.substring(i -3, i) == "<z>") i = i -3;
  let a = s.slice(0, i), j = s.indexOf(";", i) + 1;
  if (s.indexOf("</z>", j) != -1) j = s.indexOf("</z>", j) + 4;
  s = a += s.slice(j);
  i = s.indexOf("&#", i);
  }
  return s.trim();
}


const calcRSSI = (value) => {
  if (value >= 0) return `<z>&#xe81f;</z>`;
  let a = Math.round((100 - Math.abs(value)) / 0.6);
  return `${value} dBm (${a}%)`;
}


const drawer = (e) => {
  document.querySelector(`#drawer-toggle`).checked = false;
  let m = document.querySelectorAll(`.menu-item`);
  for (let i = 0; i < m.length; i++)
    m[i].classList.remove("active");
  if (e) e.classList.add('active');
}


const find = (id) => {
  for (let p = 0; p < ui.c.length; p++) {
    for (let i = 0; i < ui.c[p].length; i++) {
      if (ui.c[p][i].id == id) {
        return ui.c[p][i];
      }
    }
  }
}


const set = (id, v) => {
  ui.cfg[id] = v;
  let e = doc(id);
  if (e) {
    let i = find(id);
    if (i) {
      if (i.t == 1 || i.t == 17) {
        e.checked = v == "true" ? true : false;
      } else if (i.t == 15) {
        e.value = v;
        doc(i.id + '-r').innerHTML = `${i.l}:<span class="range-vol">${v}${i.u}</span>`;
      } else if (i.t == 20) {
        let a = document.querySelector("#" + i.id);
        if (v == "true") {
          a.classList.remove("switchOff");
          a.classList.add("switchOn");
          } else {
          a.classList.remove("switchOn");
          a.classList.add("switchOff");
        } 
      } else if (i.t == 21 || i.t == 22 || i.t == 30 || i.t == 31) {
        if (i.t < 30 ) {
          e.classList.add("anim");
          setTimeout(() => e.classList.remove("anim"), 1000);  
        }
        e.innerHTML = v;
      } else if (i.t == 23 || i.t == 24 || i.t == 33 || i.t == 34) {
        addDataChart(i.id, v);
      } else e.value = v;
    } else {
      let v2;
      if(id == "wifi") v2 = calcRSSI(v);
      else if(id == "ram") v2 = v.toString().replace(/\B(?=(\d{3})+(?!\d))/g, " ") + " Bt";
      e.innerHTML = v2 || v;
    }
  }
}


function addChart(t, id, l, a, b, c) {
  setTimeout(() => {
    let bc = c || getComputedStyle(document.documentElement,null).getPropertyValue('--main-color');
    chartArray[id] = new Chart(document.getElementById(id).getContext('2d'), {type: t == 24 ? 'bar' : 'line',
      data: {labels: a || '', datasets: [{label: l, data: b || '', borderColor: bc, borderWidth: 2, fill: true, backgroundColor: bc + "10", tension: 0.4}],},
      options: {responsive: true, maintainAspectRatio: false, elements: {point: {radius: 3, hoverRadius: 10, hoverBorderWidth: 2}},
        plugins: {tooltip: {padding: 10, backgroundColor: '#0003'}, legend: {display: true, labels: {font: {size: 16, family: 'font', weight: 'bold'}, boxWidth: 8, color: '#bbb', usePointStyle: true}}}}
    });
  }, 100);
}


function addDataChart(id, v) {
  let d = new Date();
  let t = `${d.getHours()}:${d.getMinutes() < 10 ? '0' + d.getMinutes() : d.getMinutes()}:` +
  `${d.getSeconds() < 10 ? '0' + d.getSeconds() : d.getSeconds()}`
  chartArray[id].data.labels.push(t);
  chartArray[id].data.datasets.forEach((dataset) => { dataset.data.push(v); });
  if (chartArray[id].data.labels.length > screen.width / 32) {
    chartArray[id].data.labels.shift();
    chartArray[id].data.datasets.forEach((dataset) => { dataset.data.shift(); });
  }
  chartArray[id].update({lazy: true});
};


const menu = () => {
  let a = "";
  for (let i = 0; i < ui.c.length; i++) {
    a += `<li class="menu-item" onclick="drawer(this); page=${i}; uiUpd();">${ui.c[i][0].m}</li>`;
  }
  doc("menu").innerHTML = a;
  document.querySelectorAll(`.menu-item`)[page].classList.add('active');
}


const uiUpd = () => {
  if (typeof ui.c[page] === "undefined") page = 0;
  doc("app-name").innerHTML = `<z>&#x1f50d;</z>&ensp;` + ui.an;
  doc("pageName").innerHTML = ui.c[page][0].m;  // + ` ${window.innerWidth} x ${window.innerHeight}`;
  doc("pageNameExt").innerHTML = ui.c[page][0].m + ' / ' + ui.an;
  doc("devid").innerHTML = ui.id;
  doc("fw").innerHTML = ui.fw;
  doc("page-content").innerHTML = '';
  if (ui.a == 1) {
    doc("header-r").innerHTML = `<i class="crm-icon logout-bt" title="Exit" onclick="logout()">&#x1f50d;</i>`;
  }
  for (let i = 1; i < ui.c[page].length; i++) {
    let item = ui.c[page][i];
    switch (item.t) {
      case 0: button(item); break;
      case 1: checkbox(item); break;
      case 5: input(item, 'password'); break;
      case 9: input(item, 'text'); break;
      case 10: input(item, 'color'); break;
      case 11: {
          if (ui.cfg[item.id] == "") ui.cfg[item.id] = "2021-12-02";
          input(item, 'date');
        } break;
      case 12: {
          if (ui.cfg[item.id] == "") ui.cfg[item.id] = "2021-12-02T12:21";
          input(item, 'datetime-local');
        } break;
      case 13: input(item, 'email'); break;
      case 14: input(item, 'number'); break;
      case 15: range(item); break;
      case 17: select(item); break;
      case 18: {
          if (ui.cfg[item.id] == "") ui.cfg[item.id] = "12:21";
          input(item, 'time');
        } break;
      case 20: case 21: case 22: case 23: case 24: card(item); break;
      case 30: text(item); break;
      case 31: tabl(item); break;
      case 32: hr(item); break;
      case 33: case 34: charts(item); break;
    }
  }
}


const button = (item) => {
  var a = doc("page-content").innerHTML;
  if(item.d == "r" && a.endsWith(`button')"></div>`)) {
    a = a.slice(0, a.length - 6);
  } else a += `<div class="group">`;
  a += `<input id="${item.id}" `;
  if (typeof item.s !== "undefined" || item.l.indexOf("&#x") != -1) {
    a += `style="`;
    if (typeof item.s !== "undefined" && item.s != "") a += `font-size:${item.s}px;`;
    if (item.l.indexOf("&#x") != -1) a += `font-family:'fonticon';`;
    if (typeof item.p !== "undefined" && item.p != "") a += `padding:` + item.p +`;`;
    a += `" `;
  }
  let r = "";
  if (typeof item.r !== "undefined" && item.r != "") r = item.r;
  a += `type="button" value="${item.l}" onclick="change(this.id, '${r}', 1, 'button')"></div>`;
  doc("page-content").innerHTML = a;
}

const checkbox = (item) => {
  doc("page-content").innerHTML += `<div class="group mt28"><div class="flex_row">
  <div><input id="${item.id}" type="checkbox" oninput="change(this.id, this.checked ? 'true' : 'false', 300, ${item.d == 1 ? "'checkbox'" : "''"})" ${ui.cfg[item.id] == 'true' ? 'checked' : ''}>
  <label class="switch" for="${item.id}"></label></div>
  <span class="label-chb">${item.l}</span>
  </div></div>`;
}

const input = (item, type) => {
  let a = doc("page-content").innerHTML, b = "";
  if (exist(item.d) && item.d != "") b =`pattern="${item.d}" `;
  a += `<div class="group">
  <input id="${item.id}" value="${ui.cfg[item.id]}" type="${type}" ${b}class="input-md" autocomplete="off" ${item.id == "_s" ? 'onfocusin="WST=1; getWFL()" onfocusout="WST=0" ' : ''}
  oninput="change(this.id, this.value, ${item.t > 10 && item.t < 14 ? 500 : 1000}, '${type}')">`;
  if (item.id == "_s") a += `<z class="vp" onclick="WifiList(1)">&#xe800;</z>
  <div class="wlist hide" id="wlist">
  <div><span>${lang[ui.cfg._L][18]}</span><span>${lang[ui.cfg._L][19]}</span><span>RSSI</span><span>${lang[ui.cfg._L][20]}</span></div>
  <div class="hr-cont"></div>
  <div class="witem" id="witem"><p>${lang[ui.cfg._L][21]}</p><z class="zanim">&#xe834;</z></div>
  <div class="hr-cont"></div>
  <div><input type="button" value="${lang[ui.cfg._L][22]}" onclick="WifiList(0)"></div></div>`;
  if (item.t == 5) a += `<z class="vp" onclick="viewpass('${item.id}')" id="${item.id + '-i'}">&#xe80d;</z>`;
  a += `<span class="bar"></span><label class="label-ms">${item.l}</label></div>`;
  doc("page-content").innerHTML = a;
}
const viewpass = (id) => {
  let a = document.querySelector("#" + id);
  let b = doc(id + "-i");
  if(a.type == "password") {
    a.type = "text";
    b.innerHTML = `&#xe80f;`;
  } else {
    a.type = "password";
    b.innerHTML = `&#xe80d;`;
  }
}

const range = (item) => {
  doc("page-content").innerHTML += `<div class="group">
  <input id="${item.id}" type="range" min="${item.mn}" max="${item.mx}" step="${item.s}" value="${ui.cfg[item.id]}" oninput="doc(this.id).focus(); rupd(this.id, '${item.l}', '${item.u}')" onchange="change(this.id, this.value, 100, 'range')">
  <label id="${item.id}-r" class="label-ms">${item.l}:<span class="range-vol">${ui.cfg[item.id]}${item.u}</span></label></div>`;
}
const rupd = (id, label, unit) => {
  doc(`${id}-r`).innerHTML = `${label}:<span class="range-vol">${doc(id).value}${unit}</span>`;
}

const select = (item) => {
  doc("page-content").innerHTML += `<div class="group">
  <select id="${item.id}" class="input-md" oninput="change(this.id, this.value, 1, 'select')"></select>
  <span class="bar"></span><label class="label-ms">${item.l}</label></div>`;
  for (let i = 0; i < item.o.length; i++) {
    doc(item.id).innerHTML += `<option value="${item.o[i][1]}" ${ui.cfg[item.id] == item.o[i][1] ? 'selected="selected"' : ''}>${item.o[i][0]}</option>`;
  }
}

const card = (item) => {
  var a = doc("page-content").innerHTML;
  let b = a.length, c, d = "crm-icon", e = "";
  if(b && b - a.lastIndexOf("card") < 90 && item.n != '1') {
    a = a.slice(0, b - 6);
  } else a += `<div class="group cards">`;
  if (typeof item.c !== "undefined" && item.c == "") c = "#bbb";
  else {
    c = item.c;
    if (c.indexOf("#") == -1) c = "#" + c;
  }
  a += `<div style="color: ${c};" class="card`;
  if (item.t == 22) a += ` card-bt`;
  if (item.t > 22) a += ` card-ch`;
  a += `">`;
  if (item.t < 23) {
    if(typeof item.i !== "undefined" && item.i.length > 1) {
      d += " card-icon";
      e = item.i;
    }
    a += `<h4><i class="${d}">${e}</i>${item.l}</h4>`;
  }
  if (item.t == 20) {
    if (a.indexOf("card-filter") == -1) {
      a += `<svg><filter id="card-filter"><feGaussianBlur stdDeviation="3"></feGaussianBlur><feColorMatrix values="1 0 0 0 0 0 1 0 0 0 0 0 1 0 0 0 0 0 19 -9"/></filter></svg>`;
    }
    a += `<div class="switchContainer ${ui.cfg[item.id] == 'true' ? 'switchOn' : 'switchOff'}" id="${item.id}" onclick="cardSwitcher(this.id)">
          <div class="switchBg"></div><div class="switchBtn-card"></div></div>`;
  } else if (item.t == 21) {
    a += `<p id="${item.id}" class="card-vol">${ui.cfg[item.id] || item.v}</p>`
  } else if (item.t == 22) {
    a += `<p id="${item.id}" style="cursor: pointer;" onclick="change(this.id, '', 1, 'button')" class="card-vol">${ui.cfg[item.id] || item.v}</p>`
  } else if (item.t == 23 || item.t == 24) {
    a += `<canvas id="${item.id}" height="115"></canvas>`;
  }
  a += `</div></div>`;
  doc("page-content").innerHTML = a;
  if (item.t > 22) addChart(item.t, item.id, item.l, item.a, item.b, item.c);
}

const cardSwitcher = (id) => {
  var a = document.querySelector("#" + id);
  if(a.classList.contains("switchOn")) {
    a.classList.remove("switchOn");
    a.classList.add("switchOff");
  change(id, 'false', 1, 'chb_card');
  } else {
    a.classList.remove("switchOff");
    a.classList.add("switchOn");
  change(id, 'true', 1, 'chb_card');
  }  
}

const text = (item) => {
  let a = `<div class="group text-p"`;
  if (exist(item.c)) {
    let b = item.c;
    if (b.indexOf("#") == -1) b = "#" + b;
    a += ` style="color:${b};"`;
  }
  a += `>`;
  if (item.l != "") a += `<h2>${icon(item.l)}</h2>`;
  a += `<p id="${item.id}">${icon(ui.cfg[item.id] || item.d)}</p></div>`;
  console.log();
  doc("page-content").innerHTML += a;
}

const tabl = (item) => {
  var a = doc("page-content").innerHTML;
  if (a.endsWith("</tbody></table></div>")) a = a.slice(0, a.length - 22);
  else a += `<div class="group mt28"><table><tbody>`;
  a += `<tr><th class="text-l"><pre>${item.l}</pre></th>`;
  a += `<th class="text-v"><pre`;
  if (typeof item.c !== "undefined") {
    let b = item.c;
    if (b.indexOf("#") == -1) b = "#" + b;
    a += ` style="color:${b};"`;
  }
  a += ` id="${item.id}">${ui.cfg[item.id] || item.d}</pre></th></tr></tbody></table></div>`
  doc("page-content").innerHTML = a;
}

const hr = (item) => {
  var a = doc("page-content").innerHTML;
  a += `<div class="hr-cont" style="`;
  if (item.id != "") a += `height:${item.id};`;
  if (item.l != "") a += `margin:${item.l};`;
  a += `"></div>`;
  doc("page-content").innerHTML = a;
}

function charts(item) {
  doc("page-content").innerHTML += `<div class="group"><canvas id="${item.id}" height="${item.h || 260}"></canvas></div>`;
  addChart(item.t, item.id, item.l, item.a, item.b, item.c);
}

const wifi = () => {
  for (let p = 0; p < ui.c.length; p++) {
    for (let i = 0; i < ui.c[p].length; i++) {
      if (ui.c[p][i].t == 19) {
        let a = ui.cfg._L;
        let mode = {t:17,id:"_wm",l:lang[a][0],o:[[lang[a][5],"2"],[lang[a][6],"1"],[lang[a][5] + " + " + lang[a][6],"3"]]};
        let wtc = {t:15,id:"wtc","mn":10,"mx":600,"st":1,u:lang[a][7],l:lang[a][1]};
        let ssid = {t:9,id:"_s",l:lang[a][2]};
        let pass = {t:5,id:"_p",l:lang[a][3],p:".{8,}"};
        let ap_ssid = {t:9,id:"_as",l:lang[a][4]};
        let ap_pass = {t:5,id:"_ap",l:lang[a][3],p:".{8,}"};
        ui.c[p].splice(i, 1, mode, wtc, ssid, pass, ap_ssid, ap_pass);
      }
    }
  }
}


const WifiList = (a) => {
  let b = doc("wlist");
  if (a && !WFSS) {
    b.classList.remove("anim-h");
    b.classList.add("anim-s");
    b.classList.remove("hide");
    WFSS = 1; 
    getWFL(1);
  } else {
    b.classList.remove("anim-s");
    b.classList.add("anim-h");
    WFSS = 0;
    setTimeout(() => {b.classList.add("hide")}, 1000);
  }
}

const wEnc = (e) => {
  switch (e) {
    case 0: return "X";
    case 1: return "WEP";
    case 2: return "WPA";
    case 3: return "WPA2";
    case 4: return "WPA+WPA2";
    case 5: return "WPA2-EN";
    case 6: return "DEV";
    default: return "UDF";
  }
}

const getWFL = (t) => {
  if (WFSS) setTimeout(() => {
    let xhr = new XMLHttpRequest();
    xhr.open('POST', `http://${url}/wifi`, true);
    xhr.onload = function() {
      let d = JSON.parse(xhr.responseText),
      l = doc("witem");
      if (d.s > 0) {
        l.innerHTML = "";
        for (let i = 0; i < d.s; i++) {
          let div = document.createElement("div");
          div.innerHTML = `<span>${d.n[i][0]}</span><span>${d.n[i][1]}</span><span>${d.n[i][2]} dBi</span><span>${wEnc(d.n[i][3])}</span>`;
          div.title = d.n[i][0];
          div.onclick = function() {
            doc("_s").value = d.n[i][0];
            WifiList(0);
            change("_s", d.n[i][0], 1, "");
          }
          l.appendChild(div);
        }
      } else if (d.s == 0) l.innerHTML = lang[ui.cfg._L][23];
      getWFL(2000);
    }
    xhr.send(null);
  }, t);
}


const updatepage = (d) => {
  if (d) drawer();
  let a = ui.cfg._L;
  doc("pageName").innerHTML = lang[a][15];
  doc("pageNameExt").innerHTML = lang[a][15] + ' / ' + ui.an;
  doc("page-content").innerHTML = `
  <div class="group">
  <select id="_L" class="lang" oninput="сttupd(this.value)"><option value="en">English</option><option value="de">Deutsch</option><option value="ru"><b>Русский</b></option></select></div>
  <div class="group"><div class="color-line"><input id="_C1" value="${ui.cfg._C1}" type="color" class="color" oninput="thupd(this.id, '--main-color', this.value)">${lang[a][8]}</div><div class="color-line"><input id="_C2" value="${ui.cfg._C2}" type="color" class="color cv" oninput="thupd(this.id, '--value-color', this.value)">${lang[a][9]}</div></div>
  <div class="hr-cont" style="height: 1px; margin: 20px 15% -15px"></div>
  <div class="group"><h3>${lang[a][10]}</h3><form method="POST" action="/cfgupload" enctype="multipart/form-data"><input type="file" class="file-sys" name="cfgupload"><p><input type="button" class="mt10" onclick="window.location.href = '/reset';" value="${lang[a][11]}"><input type="button" class="mt10" onclick="window.location.href = '/cfgdownload';" value="${lang[a][12]}"><input type="submit" class="mt10" value="${lang[a][13]}"></p></form></div>
  <div class="hr-cont" style="height: 1px; margin: 30px 15% -15px"></div>
  <div class="group"><h3>${lang[a][14]}</h3><form method="POST" action="/update" enctype="multipart/form-data"><input type="file" class="file-sys" name="update"><p><input type="submit" class="mt10" value="${lang[a][13]}"></p></form></div>
  `;
  const b = document.querySelector('#_L').getElementsByTagName('option');
  for (let i = 0; i < b.length; i++) {
    if (b[i].value === a) b[i].selected = true;
  }
}


const thupd = (id, p, v) => {
  change(id, v, 500, "color");
  document.documentElement.style.setProperty(p, v);
}


const сttupd = (v) => {
  change("_L", v, 1);
  updatepage();
  setTimeout(() => getUI(0), 300);
}


const post = (url, p, v, u) => {
  let formData = new FormData();
  formData.append(p, v);
  if (u) formData.append('wUPD', true);
  let xhr = new XMLHttpRequest();
  xhr.open('POST', url, true);
  xhr.send(formData);
}


const change = (id, val, delay, type) => {
  let value = doc(id).value;
  if (val != undefined) value = val;
  ui.cfg[id] = value;

  if (changeTimer != null) clearTimeout(changeTimer);
  changeTimer = setTimeout(() => {
    let element_id;
    if (type == 'button') element_id = 'BT_' + id;
    else if (type == 'chb_card') element_id = 'CR_' + id;
    else element_id = id;
    post(`http://${url}/set`, element_id, value, (type == 'select' || type == 'checkbox'));
  }, delay != undefined ? delay : 1000);
}


const notif = (type, text, tout = 5, x = 0) => {
  let w = window.innerWidth;
  SnackBar({
    status: type,
    message: text,
    timeout: tout * 1000,
    dismissible: x,
    position: w >= 400 ? "br" : "bc",
    width: w >= 400 ? "400px" : "354px"
  });
}


const logout = () => {
  let xhr = new XMLHttpRequest();
  xhr.open('GET', '/logout', true);
  xhr.send();
  setTimeout(() => window.location.reload(), 200);
}

getUI();

doc('page-content').onclick = function() {
  let e = document.querySelector(`#drawer-toggle`);
  if(e.checked == true) e.checked = false;
};


let strX, strY, strT;
document.body.addEventListener('touchstart', function(e) {
  let t = e.changedTouches[0];
  strX = t.pageX;
  strY = t.pageY;
  strT = new Date().getTime();
}, {passive: true});

let lll = doc('drawer-toggle');
document.body.addEventListener('touchend', function(e) {
    let t = e.changedTouches[0];
    let elapsedT = new Date().getTime() - strT;
    if (elapsedT <= 400 && Math.abs(t.pageY - strY) <= 100) {
      if (lll.checked) {
        if (strX - t.pageX >= 50) lll.checked = false;
      } else {
        if (t.pageX - strX >= 50) lll.checked = true;
      }
    }
}, false);

)=====";
