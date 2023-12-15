const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<title>MaxxFan Wemos Controller - Caschy</title>
<style>
.card-status{
    max-width: 800px;
     min-height: 100px;
     background: #02b875;
     padding: 30px;
     box-sizing: border-box;
     color: #FFF;
     margin:20px;
     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
}
.card-control{
    max-width: 600px;
     min-height: 250px;
     background: #3b5898;
     padding: 30px;
     box-sizing: border-box;
     color: #FFF;
     margin:20px;
     box-shadow: 0px 2px 18px -4px rgba(0,0,0,0.75);
}
.myButton {
	box-shadow: 3px 4px 0px 0px #899599;
	background:linear-gradient(to bottom, #ededed 5%, #bab1ba 100%);
	background-color:#ededed;
	border-radius:15px;
	border:1px solid #d6bcd6;
	display:inline-block;
	cursor:pointer;
	color:#000000;
	font-family:Arial;
	font-size:17px;
	padding:7px 25px;
	text-decoration:none;
	text-shadow:0px 1px 0px #e1e2ed;
}
.myButton:hover {
	background:linear-gradient(to bottom, #bab1ba 5%, #ededed 100%);
	background-color:#bab1ba;
}
.myButton:active {
	position:relative;
	top:1px;
}

</style>
<body>
<center>
<div id="container" style="width: 950px">

 <div class="card-control" style="display: inline-block">
  <h2>MaxxFan Control</h2><br>
  <a href="#" onClick="getReq('/fanOPEN');" class="myButton">OPEN</a>
  <a href="#" onClick="getReq('/fanCLOSE');" class="myButton">CLOSE</a>
  <br><br>
  <a href="#" onClick="getReq('/fanON');" class="myButton">ON</a>
  <a href="#" onClick="getReq('/fanOFF');" class="myButton">OFF</a>
  <br><br>
  <a href="#" onClick="getReq('/fanIN');" class="myButton">IN</a>
  <a href="#" onClick="getReq('/fanOUT');" class="myButton">OUT</a>
  <br><br>
  <p>Set Fan Speed:</p>
  <select id="mySpeed" onchange="getReqSpeed()">
   <option value="fan10p">10%</option>
   <option value="fan20p">20%</option>
   <option value="fan30p">30%</option>
   <option value="fan40p">40%</option>
   <option value="fan50p">50%</option>
   <option value="fan60p">60%</option>
   <option value="fan70p">70%</option>
   <option value="fan80p">80%</option>
   <option value="fan90p">90%</option>
   <option value="fan100p">100%</option>
  </select>
<!--  <p>Set Temperature:</p>
  <select id="myTemp" onchange="getReqTemp()">
   <option value="fan60t">60&#176;F</option>
   <option value="fan65t">65&#176;F</option>
   <option value="fan70t">70&#176;F</option>
   <option value="fan75t">75&#176;F</option>
   <option value="fan80t">80&#176;F</option>
   <option value="fan85t">85&#176;F</option>
  </select>-->
  <p>Set Timer Function:</p>
  <select id="myTimeFunc">
   <option value="timerFOff">Off</option>
   <option value="timerFOn">On</option>
   <option value="timerFOpen">Open</option>
   <option value="timerFClose">Close</option>
   <option value="timerF10">10%</option>
   <option value="timerF50">50%</option>
  </select>
  <p>Set Timer:</p>
  <select id="myTime" onchange="getReqTimer()">
   <option value="timerOff">OFF</option>
   <option value="timer15m">15 minutes</option>
   <option value="timer30m">30 minutes</option>
   <option value="timer1h">1 hour</option>
   <option value="timer2h">2 hours</option>
   <option value="timer3h">3 hours</option>
   <option value="timer4h">4 hours</option>
  </select>
 </div>

</div>
<div class="card-status" style="display: inline-block">
<h3 id="timerLabel">Timer Not Set</h3>
</div>
</center>

<script>
function getReqTemp() {
    var y = "/"+document.getElementById("myTemp").value;
    var xchttp3 = new XMLHttpRequest();
    xchttp3.open( "GET", y, false ); // false for synchronous request
    xchttp3.send(null);
}
function getReqTimer() {
    var z = "/"+document.getElementById("myTime").value;
    var xchttp4 = new XMLHttpRequest();
    xchttp4.open( "GET", z, false ); // false for synchronous request
    xchttp4.send(null);
}
function getReqSpeed() {
    var x = "/"+document.getElementById("mySpeed").value;
    var xchttp2 = new XMLHttpRequest();
    xchttp2.open( "GET", x, false ); // false for synchronous request
    xchttp2.send(null);
}
function getReq(theUrl) {
    var xchttp = new XMLHttpRequest();
    xchttp.open( "GET", theUrl, false ); // false for synchronous request
    xchttp.send(null);
}
function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var respon = this.responseText.split("|");
      document.getElementById("timerLabel").innerText = "Set for:"+respon[0]+" || "+respon[1]+"ms left or "+respon[1]/60000+"min left";
	  //document.getElementById("myTime").selectedIndex=2;
    }  
  };
  xhttp.open("GET", "readDATA", true);
  xhttp.send();
}
setInterval(function() {
  // Call a function repetatively with 5 Second interval
  getData();
}, 5000); //10000mSeconds update rate
</script>

</body>
</html>
)=====";
