const char style_css[] PROGMEM = R"=====(

/*@font-face {
  font-family: "font";
  src:  url(fonts/font.ttf) format("truetype");
  font-style: normal;
}*/

@font-face {
  font-family: "3ds";
  src:  url(fonts/3ds.ttf) format("truetype");
  font-style: normal;
}

@font-face {
  font-family: "fonticon";
  src: url(fonts/fonticon.woff2) format("woff2");
  font-style: normal;
}

:root {
  /*--main-color: #0098d9;*/
  --header-color: var(--main-color);
  --headings-color: #bbb;
  --value-color: #fff;
  --menu-active: var(--main-color);
  --menu-background: #222;
  --background: #333;
  --label-font-size: 16px;
  --value-font-size: 18px;
  --drawer-left: 250px;
}

* {
  font-family: sans-serif;
  -webkit-box-sizing: border-box;
  -moz-box-sizing: border-box;
  -o-box-sizing: border-box;
  box-sizing: border-box;
  -webkit-transition: .3s ease-in-out;
  -moz-transition: .3s ease-in-out;
  -o-transition: .3s ease-in-out;
  transition: .3s ease-in-out;
  margin: 0;
  padding: 0;
  -webkit-text-size-adjust: none;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  outline: none;
}

html, body {
  height: 100%;
  overflow: hidden;
  background: var(--background);
}

header {
  font-family: "3ds";
  width: 100%;
  position: fixed;
  left: 0px;
  background: var(--header-color);
  padding: 10px 10px 10px 50px;
  font-size: 28px;
  color: #fff;
  line-height: 30px;
  z-index: 1;
  box-shadow: 0 15px 20px 0px rgb(33 33 33);
}

a {
  color: currentcolor;
}

#header-r {
  position: absolute;
  right: 15px;
  top: 9px;
  color: #fff;
  z-index: 1;
}

#app-name {
  font-size: 28px;
  padding: 10px 0px 10px 0px;
  color: #ffffff;
  text-align: center;
  cursor: pointer;
  /*margin: 0 15px;*/
}

z, .crm-icon {
  font-family: "fonticon";
  font-style: normal;
  font-variant: normal;
  font-weight: normal;
  display: inline-block;
  text-decoration: inherit;
  text-align: center;
}

.zanim {
  -moz-animation: spin 2s infinite linear;
  -o-animation: spin 2s infinite linear;
  -webkit-animation: spin 2s infinite linear;
  animation: spin 2s infinite linear;
  display: inline-block;
}

@-moz-keyframes spin {
  0% {-moz-transform: rotate(0deg); -o-transform: rotate(0deg); -webkit-transform: rotate(0deg); transform: rotate(0deg);}
  100% {-moz-transform: rotate(359deg); -o-transform: rotate(359deg); -webkit-transform: rotate(359deg); transform: rotate(359deg);}
}

@-webkit-keyframes spin {
  0% {-moz-transform: rotate(0deg); -o-transform: rotate(0deg); -webkit-transform: rotate(0deg); transform: rotate(0deg);}
  100% {-moz-transform: rotate(359deg); -o-transform: rotate(359deg); -webkit-transform: rotate(359deg); transform: rotate(359deg);}
}

@-o-keyframes spin {
  0% {-moz-transform: rotate(0deg); -o-transform: rotate(0deg); -webkit-transform: rotate(0deg); transform: rotate(0deg);}
  100% {-moz-transform: rotate(359deg); -o-transform: rotate(359deg); -webkit-transform: rotate(359deg); transform: rotate(359deg);}
}

@-ms-keyframes spin {
  0% {-moz-transform: rotate(0deg); -o-transform: rotate(0deg); -webkit-transform: rotate(0deg); transform: rotate(0deg);}
  100% {-moz-transform: rotate(359deg); -o-transform: rotate(359deg); -webkit-transform: rotate(359deg); transform: rotate(359deg);}
}

@keyframes spin {
  0% {-moz-transform: rotate(0deg); -o-transform: rotate(0deg); -webkit-transform: rotate(0deg); transform: rotate(0deg);}
  100% {-moz-transform: rotate(359deg); -o-transform: rotate(359deg); -webkit-transform: rotate(359deg); transform: rotate(359deg);}
}

.logout-bt {
  font-size: 30px;
  cursor: pointer;
}

.logout-bt:hover {
  text-shadow: 0 0 4px #ffffff;
}

#page-content {
  margin-left: 0px;
  margin-top: 51px;
  margin-inline: auto;
  width: 100%;
  height: calc(100% - 50px);
  overflow-x: hidden;
  padding: 5px 25px 50px;
  -webkit-overflow-scrolling: touch;
}

.group {
  align-content: center;
  text-align: center;
  position: relative;
  margin-top: 35px;
}

.lang {
  width: 200px; 
  border-radius: 5px; 
  font-size: calc(var(--value-font-size) - 1px); 
  border-color: var(--main-color); padding: 5px;
}

.lang:hover {
  box-shadow: 0 0 15px 1px var(--main-color);
}

.color {
  width: 70px;
  height: 30px !important;
  padding: 5px;
  border-radius: 5px;
  box-shadow: 0 0 5px var(--main-color);
  margin: 0 auto 5px !important;
}

.cv {
  box-shadow: 0 0 5px var(--value-color);
}

.color:hover {
  box-shadow: 0 0 15px 1px var(--main-color);
}

.cv:hover {
  box-shadow: 0 0 15px 1px var(--value-color);
}

.vp {
  color: var(--value-color);
  position: absolute;
  top: 15px;
  right: 10px;
  cursor: pointer;
}
  
.color-line {
  display: inline-grid;
  color: var(--headings-color);
  width: 150px;
  font-size: var(--label-font-size);
  margin: 10px;
}

.file-sys {
  box-sizing: border-box;
  margin: 0 0 20px;
  border-radius: 7px;
  box-shadow: 0 0 5px var(--main-color);
  padding: 0px !important;
}

h3 {
  font-size: x-large;
  margin: 0 0 10px; 
  text-align: center;
  color: var(--headings-color);
  letter-spacing: .10em;
}

h4 {
  font-size: 1.2rem;
  text-transform: uppercase;
}

.mt10 {
  margin: 10px 20px !important;
}

.mt20 {
  margin-top: 20px;
} 

.mt28 {
  margin-top: 28px;
}

.anim {
  animation: doAnim 1s both;
}

@keyframes doAnim {
  0%{opacity:0;}
  100%{opacity:1;}
}

.ke {
  font-family: "3ds";  
  margin: 0 0 8px;
  font-size: 17px;
  letter-spacing: normal;
}

.ke > span:nth-child(1) {
  color:red;
  font:inherit;
  margin-right: 6px;
}

.ke > span:nth-child(2) {
  color:white;
  font:inherit;
}

/* MENU */
#sys-menu {
  width: var(--drawer-left);
  text-align: center;
  font-size: 25px;
  color: #aaa;
  padding: 0 0 10px;
}

.sys-menu-item {
  margin: 0 5px;
  cursor: pointer;
}

.sys-menu-item:hover {
  color: #fff;
  text-shadow: 0 0 10px var(--main-color);
}

.sys-t {
  height: calc(100% - 70px);
  border-spacing: 0;
}

.td1 {
  vertical-align: top;
}

.td2 {
  height: 290px;
  vertical-align: bottom;
}

#sys-info {
  color: #aaa;
  -webkit-transition: none;
  -moz-transition: none;
  -o-transition: none;
  transition: none;
  cursor: default;
  padding: 0px 15px 15px;
}

.sys-info-i {
  font-size: 16px;
  margin-top: 7px;
}

.sys-info-t {
  margin: 0 5px 0 15px;
  font-size: 14px;
}

.wf {
  margin-left: -3px;
}

#drawer {
  position: fixed;
  top: 0;
  left: calc(0px - var(--drawer-left));
  height: 100%;
  width: var(--drawer-left);
  background: var(--menu-background);
  overflow-x: hidden;
  -webkit-overflow-scrolling: touch;
  z-index: 2;
}

#drawer-toggle {
  position: absolute;
  opacity: 0;
}

#drawer-toggle-label {
  -webkit-touch-callout: none;
  -webkit-user-select: none;
  -khtml-user-select: none;
  -moz-user-select: none;
  -ms-user-select: none;
  user-select: none;
  left: 0px;
  height: 50px;
  width: 50px;
  display: block;
  position: fixed;
  background: rgba(255,255,255,.0);
  z-index: 2;
}

#drawer-toggle-label:before {
  content: '';
  display: block;
  position: absolute;
  height: 3px;
  width: 24px;
  background: #ffffff;
  left: 13px;
  top: 18px;
  box-shadow: 0 6px 0 #ffffff, 0 12px 0 #ffffff;
}

#drawer-toggle:checked ~ #drawer-toggle-label, #drawer-toggle:checked ~ header {
  left: var(--drawer-left);
}

#drawer-toggle:checked ~ #drawer {
  left: 0px;
  box-shadow: 0 0 50px rgba(0,0,0,0.8);
}

#drawer-toggle:checked ~ #page-content {
  margin-left: var(--drawer-left);
}

#drawer ul {
  list-style-type: none;
}

#drawer hr.menu-hr {
  border: 0;
  height: 1px;
  margin: 0px 15px 10px;
  background-image: -webkit-linear-gradient(left,var(--menu-background),var(--main-color),var(--menu-background));
}

#drawer ul .menu-item {
  display: block;
  font-size: 18px;
  padding: 6px 15px 6px 15px;
  color: var(--headings-color);
  text-decoration: none;
}

#drawer ul .menu-item:hover {
  color: white;
  cursor: pointer;
  -webkit-transition: .3s cubic-bezier(0.68, -0.55, 0.27, 1.55);
  -moz-transition: .33s cubic-bezier(0.68, -0.55, 0.27, 1.55);
  -o-transition: .3s cubic-bezier(0.68, -0.55, 0.27, 1.55);
  transition: .3s cubic-bezier(0.68, -0.55, 0.27, 1.55);
}

#drawer ul .active {
  color: white;
  background: linear-gradient(90deg, var(--main-color), #0000);
}

#drawer::-webkit-scrollbar {
  width: 2px;
}

#drawer::-webkit-scrollbar-thumb {
  background-image: -webkit-linear-gradient(top,var(--menu-background),var(--main-color),var(--menu-background));
}

#page-content::-webkit-scrollbar {
  width: 6px;
}

#page-content::-webkit-scrollbar-thumb {
  background-image: -webkit-linear-gradient(top,var(--background),var(--main-color),var(--background));
}

/*Для больших экранов*/
/*по ширине*/
@media (min-width: 600px) {
  header {
    left: var(--drawer-left);
    padding: 10px 20px;
    width: calc(100% - var(--drawer-left));
  }
  
  #page-content {
    margin-left: var(--drawer-left);
    width: calc(100% - var(--drawer-left));
  }

  #drawer {
    left: 0px;
    box-shadow: 0 0 50px rgba(0,0,0,0.8);
  }

  #drawer-toggle-label:before {
    display: none;
  }

  .switchContainer {
    transform: scale(2.5);
  }

  .file-sys {
    width: 400px;
  }
}
/*по высоте*/
/*@media (min-height: 500px) {
  #sys-info {
    display: block;
  }
}*/

/*Для маленьких экранов*/
/* по ширинет*/
@media (max-width: 600px) {
  #drawer-toggle:checked ~ #header-r {
    display: none;
  }
  
  .switchContainer {
    transform: scale(2);
  }

  .file-sys {
    width: 290px;
  }
}
/*по высоте*/
/*@media (max-height: 500px) {
  #sys-info {
    display: none;
  }
}*/

/* TAB ELEMENT */
input, select{
  background: var(--background);
  color: var(--value-color);
}

input{
  padding: 10px;
}

input:invalid {
  color: red
}

/* фон автозаполнения */
/*input:-webkit-autofill,
input:-webkit-autofill:hover, 
input:-webkit-autofill:focus,
textarea:-webkit-autofill,
textarea:-webkit-autofill:hover,
textarea:-webkit-autofill:focus,
select:-webkit-autofill,
select:-webkit-autofill:hover,
select:-webkit-autofill:focus {
  -webkit-text-fill-color: var(--value-color);
  -webkit-box-shadow: 0 0 0px 1000px var(--background) inset;
  transition: background-color 5000s ease-in-out 0s;
}*/

input[type="number"] {
  -moz-appearance: textfield;
}

input::-webkit-outer-spin-button, input::-webkit-inner-spin-button {
  -webkit-appearance: none;
  margin: 0;
}

select {
  padding: 10px 6px;
}

input[type="date"] {
  padding: 10px 7px;
}

input[type="time"] {
  padding: 10px 7px;
}

input[type="datetime-local"] {
  padding: 10px 7px;
}

/* LABEL & VALUE */
.label-ms {
  white-space: nowrap;
  font-size: var(--label-font-size);
  color: var(--headings-color);
  position: absolute;
  pointer-events: none;
  left: 10px;
  top: -18px;
}

.input-md {
  -moz-appearance: none;
  font-size: var(--value-font-size);
  display: block;
  width: 100%;
  border: none;
  border-bottom: 1px solid #ccc;
}

.input-md:hover {
  border-bottom-color: var(--main-color);
}

.input-md:focus ~ .bar:before, .input-md:focus ~ .bar:after {
  width: 50%;
}

.bar {
  position: relative;
  display: block;
  width: 100%;
}

.bar:before, .bar:after {
  content: "";
  height: 2px;
  width: 0;
  bottom: 0;
  position: absolute;
  background: var(--main-color);
  transition: 0.6s ease all;
  -moz-transition: 0.6s ease all;
  -webkit-transition: 0.6s ease all;
}

.bar:before {
  left: 50%;
}

.bar:after {
  right: 50%;
}

/* RANGE */
input[type=range] {
  -webkit-appearance: none;
  width: 100%;
  margin-top: 25px;
  padding: 7px 10px 6px;
}

input[type=range]::-webkit-slider-runnable-track {
  height: 2px;
  border: none;
  padding: 0px 10px;
  background: var(--main-color);
}

input[type=range]::-moz-range-track { /*FireFox*/
  height: 2px;
  border: none;
  padding: 0px 10px;
  background: var(--main-color);
}

input[type=range]::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: 18px;
  height: 18px;
  border-radius: 50%;
  border: 2px solid rgb(0 0 0 / 20%);
  box-shadow: 0 0px 11px 0 var(--main-color);
  margin-top: -8px;
  background: var(--main-color);
  cursor: pointer;
}

input[type=range]::-moz-range-thumb { /*FireFox*/
  -webkit-appearance: none;
  appearance: none;
  width: 18px;
  height: 18px;
  border-radius: 50%;
  border: 2px solid rgb(0 0 0 / 20%);
  box-shadow: 0 0px 11px 0 var(--main-color);
  margin-top: -8px;
  background: var(--main-color);
  cursor: pointer;
}

input[type=range]::-webkit-slider-thumb:hover {
  -webkit-appearance: none;
  -webkit-transition: .2s ease-in-out;
  -moz-transition: .2s ease-in-out;
  -o-transition: .2s ease-in-out;
  transition: .2s ease-in-out;
  appearance: none;
  width: 22px;
  height: 22px;
  border-radius: 25%;
  border: 2px solid #ccc9;
  margin-top: -10px;
  background: var(--main-color);
  opacity: .8;
  cursor: pointer;
}

input[type=range]::-moz-range-thumb:hover {   /*FireFox*/
  -webkit-appearance: none;
  -webkit-transition: .2s ease-in-out;
  -moz-transition: .2s ease-in-out;
  -o-transition: .2s ease-in-out;
  transition: .2s ease-in-out;
  appearance: none;
  width: 22px;
  height: 22px;
  border-radius: 25%;
  border: 2px solid #ccc9;
  margin-top: -10px;
  background: var(--main-color);
  opacity: .8;
  cursor: pointer;
}

input[type=range]:valid ~ .label-ms {
  top: -9px;
  color: var(--headings-color);
}

.range-vol {
  padding: 0 10px;
  font-size: var(--value-font-size);
  color: var(--value-color);
}

/* COLOR */
input[type="color"] {
  -webkit-appearance: none;
  margin-block-start: 45px;
  border: none;
  height: 43px;
  cursor: pointer;
}

input[type="color"]::-webkit-color-swatch-wrapper {
  padding: 0;
}

input[type="color"]::-webkit-color-swatch {
  border: none;
}

input[type="color"]~ .label-ms {
  top: -28px;
}

/* BUTTON */
input[type="button" i]:hover,  input[type="submit" i]:hover{
  border: 2px solid var(--main-color);
  box-shadow: inset 0 0px 15px 0 var(--main-color);
  color: white;
}

input[type="button" i]:active,  input[type="submit" i]:active{
  -webkit-transition: .1s ease-in-out;
  -moz-transition: .1s ease-in-out;
  -o-transition: .1s ease-in-out;
  transition: .1s ease-in-out;
  box-shadow: 0 0 0 1px rgb(0 0 0 / 15%) inset;
}

input[type="button"], input[type="submit"] {
  -webkit-appearance: button;
  cursor: pointer;
  letter-spacing: .10em;
  background: none;
  text-transform: uppercase;
  color: var(--headings-color);
  font-size: initial;
  border: 2px solid var(--main-color);
  border-radius: 6px;
  box-shadow: 0 0px 11px 0 var(--main-color);
  padding: .3em 1.5em;
  margin: 0 20px 10px;
}

.flex_row {
  display: flex;
  flex-direction: row;
}

/* CHECKBOX */
.switch {
  position: relative;
  display: block;
  cursor: pointer;
  width: 55px;
  height: 25px;
  margin: 0 20px 0 10px;
  box-shadow: 0 1px 10px var(--main-color);
  border-radius: 20px;
  border: 1px solid #ccc;
}

.switch::after {
  content: '';
  position: absolute;
  top: 2px;
  left: 2px;  
  width: 19px;
  height: 19px;
  border-radius:90px;
  background: white;
  transition: all 0.3s;
}

label:active:after {
  width: 60%;
}

input[type="checkbox" i]:checked + .switch::after {
  left: calc(100% - 2px);
  transform: translateX(-100%);
}

input[type="checkbox" i]:checked + .switch {
  background-color: var(--main-color);
}

input[type="checkbox"] {
  display: none;
}

/*input[type="checkbox"] ~ .label-chb {*/
.label-chb {
  color: var(--headings-color);
  font-size: var(--label-font-size);
  text-align: justify;
  position: relative;
  top: 4px;
}

/* CARD */
.cards {
  font-family: "font";
  margin-inline: auto;
  max-width: 1500px;
  display: grid;
  grid-gap: 2rem;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
}
  
.card {
  display: inline-block;
  text-align: center;
  box-shadow: 0 0 10px 2px rgb(255 255 255 / 37%);
  padding: 30px 0 10px;
  border-radius: 10px;
  cursor: default;
}

.card-ch {
  padding: 5px;
  min-height: 180px;
}

.card:hover {
  box-shadow: 0 0 15px 5px var(--main-color);
}

.card-bt:active {
  width: 97%;
  height: 97%;
  margin: auto;
}

.card-icon {
  margin: 0 7px 0 0;
}

.card-vol {
  padding: 20px 0;
  font-size: 2.8rem;
}
  
::-moz-selection {
  background: #fff2;
}

::selection {
  background: #fff2;
}

svg {
  display: none;
}

.switchContainer {
  display: inline-block;
  filter: url("#card-filter");
  -webkit-filter: url("#card-filter");
  position: relative;
  cursor: pointer;
  margin: 47px;
}
  
.switchBg {
  width:90px;
  height: 7px;
}

.switchBtn-card {
  width: 30px;
  height: 30px;
  border-radius: 50%;
  display: inline-block; 
  position: absolute;
  left: 0px;
  top: -12px;
  transform: translate3d(0,0,0);
}

.switchBg, .switchBtn-card {
  background: #ddd;
  transition: 300ms all ease;
}
  
.switchContainer.switchOn .switchBtn-card, .switchContainer.switchOn .switchBg {
  background: var(--main-color);
}

.switchContainer.switchOn .switchBtn-card {
  animation: switchOn 500ms linear forwards;
 }

.switchContainer.switchOff .switchBtn-card {
  animation: switchOff 500ms linear forwards;
 }

@keyframes switchOn { 
  0%{transform: scale3d(1,1,1) translate3d(0px,0,0); transform-origin: left center;} 
  40%{transform: scale3d(1.4,0.7,0) translate3d(30px,0,0); transform-origin: right center;}
  70%{transform: scale3d(0.9,1.15,1) translate3d(75px,0,0); }
  100%{transform: scale3d(1,1,1) translate3d(60px,0,0); }
}

@keyframes switchOff { 
  0%{transform: scale3d(1,1,1) translate3d(60px,0,0); transform-origin: right center;} 
  40%{transform: scale3d(1.4,0.7,0) translate3d(30px,0,0); transform-origin: right center;}
  70%{transform: scale3d(0.9,1.15,1) translate3d(-10px,0,0); }
  100%{transform: scale3d(1,1,1) translate3d(0px,0,0); }
}

/* TEXT */
.text-p {
  border-radius: 10px;
  background: #fff1;
  padding: 10px 15px;
  font-size: calc(var(--label-font-size) + 1px);
  box-shadow: 0 0 3px 1px rgb(255 255 255 / 40%);
  text-align: justify;
  margin: 28px 5px 0;
  cursor: default;
} 

.text-l {
  width: 50%;
  background: linear-gradient(270deg, #fff2, #0000 85%);
  text-align: right;
}

.text-v {
  background: linear-gradient(90deg, #fff2, #0000 85%); 
    text-align: left;
}

table {
  border: 0;
  border-spacing: 5px;
  width: 100%;
  cursor: default;
}

th {
  color: var(--headings-color);
  border-radius: 7px;
  font-size: var(--label-font-size);
}

pre {
  padding: 4px 10px 5px;
  white-space: pre-wrap;
}

/*Разделитель*/
.hr-cont {
  border: 0;
  background-image: -webkit-linear-gradient(left,var(--background),var(--main-color),var(--background));
}

/* WiFi поиск */
.wlist {
  box-shadow: 0 0 15px 5px var(--main-color);
  background: var(--background);  
  border-radius: 12px;
  min-width: 310px;
  max-width: 480px;
  position: absolute;
  top: -170px;
  left: 0;
  right: 0;
  margin: auto;
  z-index: 3; 
  font-size: calc(var(--label-font-size) - 2px);  
  color: white;
  text-align: left; 
}

.wlist > div:nth-child(1){
  display: flex;
  justify-content: space-between;
  flex-direction: row;
  margin: 10px 20px;
}

.wlist > div:nth-child(2) {
  height:1px;
  margin:-4px 0 10px;
}

.wlist > div:nth-child(4) {
  height: 1px;
  margin: 0px 0 10px;
}

.wlist > div:nth-child(5) {
  text-align: center;
}

.wlist > input {
  margin: 0 !important;
}

.witem {
  min-height: 200px;
  max-height: 320px;
  overflow-y: auto;
  overflow-x: hidden;
  margin: 0 2px 15px;
}


.witem::-webkit-scrollbar {
  width: 2px;
}

.witem::-webkit-scrollbar-thumb {
  background-image: -webkit-linear-gradient(top,var(--background),var(--main-color),var(--background));
}

.witem > p {
  position: relative;
  top: 50px;
  text-align: center;
}

.witem > z {
  position: absolute;
  top: 130px;
  left: 0;
  right: 0;
  font-size: 25px;
  text-shadow: 0 0 10px var(--main-color);
}

.witem > div {
  display: flex;
  justify-content: space-between;
  flex-direction: row;
  align-items: center;
  padding: 8px 20px;
  -webkit-transition: 0.1s ease-in-out;
  -moz-transition: .1s ease-in-out;
  -o-transition: .1s ease-in-out;
  transition: .1s ease-in-out;
}

.witem > div:hover {
  color: var(--main-color);
  cursor: pointer;
}

.witem > div > span + span + span + span, .wlist > div > span + span + span + span{
  width: 120px;
}

.witem > div > span + span + span, .wlist > div > span + span + span {
  width: 75px;
}

.witem > div > span + span, .wlist > div > span + span {
  width: 60px;
  text-align: center;
}

.witem > div > span, .wlist > div > span {
  width: 150px;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
}

.hide {
  display: none;
}

.anim-s {
  animation: AnimS  .3s both ;
}

@keyframes AnimS {
  0%{opacity:0;top:-190px;}
  100%{opacity:1;top:-170px;}
}

.anim-h {
  animation: AnimH  .3s both ;
}

@keyframes AnimH {
  0%{opacity:1;top:-170px;}  
  100%{opacity:0;top:-150px;}
}

)=====";
