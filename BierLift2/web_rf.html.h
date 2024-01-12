const char rf_html[] PROGMEM = R"=====(
<!DOCTYPE HTML>
<html lang="en">
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="mobile-web-app-capable" content="yes">
    <meta name="apple-mobile-web-app-capable" content="yes">
    <meta name="msapplication-starturl" content="/">
    <link rel="manifest" href="manifest.json">
    <link rel="stylesheet" href="notif.css">
    <style>
    	:root{--mail-color:#0098d9;}
    	body{background-color:#333;}
      body{color:#ccc;}
    	p{text-align:center;font-size:25px;font-weight:bold;margin:40px 0 20px;}
    	label{font-size:20px;margin-bottom:5px;}
    	.gr{display: flex;flex-direction: column;align-items:flex-start;padding:10px;}
      .indt{box-shadow:0 0 5px var(--mail-color);width:300px;height:36px;font-size:20px;border-radius: 10px;outline: none;background-color: inherit;color: white;border: none;padding-inline: 10px;}
      .main{display: flex;flex-direction: column;align-items: center;}
    	.but{-webkit-appearance: button;cursor: pointer;letter-spacing:0.3em;background: none;text-transform: uppercase;color: white;font-size: initial;border: 2px solid var(--mail-color);border-radius: 25px;box-shadow: 0 0px 11px 0 var(--mail-color);padding: 0.7em 3em;margin-top: 30px;}
    	.but:hover{border: 2px solid var(--mail-color);box-shadow:inset 0 0px 15px 0 var(--mail-color);color: white;}
    	.but:active{-webkit-transition:.1s ease-in-out;-moz-transition:.1s ease-in-out;-o-transition: .1s ease-in-out;transition: .1s ease-in-out;box-shadow: 0 0 0 1px rgb(0 0 0 / 15%) inset;}
    </style>
  </head>
  <body>
	<p>RF CODE SENDER</p>
    <div class="main">
		<div class="gr">
			<label>Device IP</label>
			<input type="text" class="indt" id="devip">
		</div>
		<div class="gr">
			<label>Transmitter data port</label>
			<input type="number" class="indt" value="25" id="rfport">
		</div>
    <div class="gr">
      <label>Number of send packages (max 100)</label>
      <input type="number" class="indt" value="40" id="npac">
    </div>
		<div class="gr">
			<label>Button code</label>
			<input type="number" class="indt" id="butcode">
		</div>
		
		<input type="button" class="but" onclick="send()" value="SEND">
	</div>
	
	<script defer src="notif.js"></script>
	<script>
    document.getElementById("devip").value = window.location.host;
  
		const notif = (text, color = "Green", t = 5) => {
		  let w = window.innerWidth;
		  SnackBar({
			status: color,
			message: text,
			timeout: t * 1000,
			dismissible: true,
			position: w >= 400 ? "br" : "bc",
			width: w >= 400 ? "400px" : "354px"
		  });
		}
		
		function send() {
			let rfport = document.getElementById("rfport").value,
          butcode = document.getElementById("butcode").value,
			    devip = document.getElementById("devip").value,
			    npac = document.getElementById("npac").value;
      if(!npac || npac > 100) npac = 50;
					
			let url = "http://" + devip + "/api?sendCode=" + butcode + "&sendPacs=" + npac;
			if (rfport != "") url += "&sendPort=" + rfport;
			
			if (devip != "") {
				if (rfport != "") {
					try {
            notif(`Sending code ${butcode} ...`, "Green", 2);
						let xhr = new XMLHttpRequest();
						xhr.open('GET',  url, true);
						xhr.onload = function() {
              let j = JSON.parse(xhr.responseText),
                  m = `Request status: ${j["stateRequest"]}<br>
                      Lead time: ${j["leadTime_ms"]} ms<br>
                      Send code: ${j["sendCode"]}<br>
                      Transmitter port: ${j["sendPort"]}<br>
                      Number of packages: ${j["sendPacs"]}<br>
                  `;
							notif(m);
						}
						xhr.send(true);
					} catch (e) {
						notif("Unsuccessful request, please check the correctness of the entered data.<br>Error: " + e, "Red");
					}
				} else notif("Transmitter data port not set!", "Red");
			} else notif("Device IP not set!", "Red");
		}
	</script>
  </body>
</html>
)=====";
