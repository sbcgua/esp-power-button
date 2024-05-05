#ifndef WEBPAGE_H
#define WEBPAGE_H

static const char* index_page = R"=====(
<!DOCTYPE HTML>
<html>
  <head>
    <link rel='icon' href='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAACXBIWXMAAAEwAAABMAGW7EZ5AAAAGXRFWHRTb2Z0d2FyZQB3d3cuaW5rc2NhcGUub3Jnm+48GgAAAg1JREFUOI2Fk89LVFEUxz/n+hIXNQhBP9AKbTEzTxeB/dgEElFQ0CoNinzThMuEXLQJ6nUbbe+iIC2aSXJh7WoRRuLCTRBEZDNagZWQ9heMMDPvtPC96WEjndX9fr/nnPu953BFVWkUV0cX9lWrZk6QSmVbrXfqZvfvRnkmDgbulfZmc5+PAFSrzklBOoFkc8X0Anh26bBnS93xGokceLniMQnkNdCqwuCGyCMAEbmkyBoavAVqoBcLvvui7sAbWUhLIDNAa0gmNlsNCBxAAAfk2cDd0ikAYy1Gak3j/C2a7ND02OYGk7fTb0BtCJuN8jBrv7eYZYp9wPFQeFd201nfJ2g0sILv3gGdCmFHIOUho4gXJSjm2nQ/NQCjslLnNaifHZpuAJUNXvoNcDTUvj71k++jxLyfmgE5Z9AzBd+dj/jHfvIX8AFA0C4nGhyw+q/l1KtGTwHWI0MGWAvBQRFki4J6nLBzDtAVwp8GiOy1ZWzp7P8a7Jfd54GdIZwziJmIRFUeZEa/tG1V7I0sdYgSrVhBJ0RVydjF56B9obCC6FCnui+jdVqLWabYp8gYsAdA0Hzed7Oiqly23xJNVGaBntiFq4p8FNSAHALdFdPmy4kdp6eH28v1v3DBFre3IPcFPLYOVXRiPZG4Pj3cXobYZ4riSm6xh0AHFXqBA4AK/FB01hgz/uRW6lM8/w/d/8Cok0civAAAAABJRU5ErkJggg=='>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style type="text/css">
      div.container {
        width: 200px;
      }
      div.power {
        width: 100px;
      }
      div.button {
        display: block;
        height: 100px;
        width: 100px;
        line-height: 100px;
        border-radius: 50%;
        border: 2px solid #f5f5f5;
        color:#f5f5f5;
        cursor: pointer;
        text-align: center;
        text-decoration: none;
        background-color: #555777;
        box-shadow: 0 0 3px gray;
        font-size: 20px;
        font-weight: bold;
        overflow-wrap: break-word;
      }
      div.stats {
        margin: 1em 0px;
      }
      div.stats p {
        color: grey;
        font-family: monospace;
        margin: 0px;
      }
      div.stats label {
        display: inline-block;
        width: 5em;
        padding-left: 4px;
      }
      div.advanced {
        background-color: #f0f0f0;
        border: 1px solid #e0e0e0;
        border-radius: 3px;
        font-family: sans-serif;
        font-size: small;
      }
      div.advanced h2 {
        text-transform: uppercase;
        font-size: x-small;
        font-family: sans-serif;
        margin: 4px 4px;
        color: #db9410;
      }
      div.advanced ul {
        padding-left: 1em;
      }
      div.advanced li {
        list-style-type: none;
        padding: 0;
        margin: 4px;
      }
      div#log {
        border-radius: 3px;
        font-size: small;
        font-family: sans-serif;
        color: #707070;
        margin: 1em 0;
        padding: 4px 4px;
        display: none;
      }
      .state-pending {
        background-color: #e0e0e0;
        display: block !important;
      }
      .state-ok {
        background-color: #a5f9ba;
        display: block !important;
      }
      .state-error {
        background-color: #f7c9c9;
        display: block !important;
      }
    </style>
  </head>
  <body>
    <div class="container">
      <div class="power">
        <div class="button" onclick="pressPower()">Power</div>
      </div>
      <div class="stats">
        <div id="log"></div>
        <p><label>Chip ID:</label>%CHIP_ID%</p>
        <p><label>Uptime:</label>%UP_TIME%</p>
      </div>
      <div class="advanced">
        <h2>Advanced</h2>
        <ul>
          <li>Do <a href='#' onclick="pressPowerLong()">long press</a></li>
        </ul>
      </div>
    </div>
    <script type="text/javascript">
      const logDiv = document.getElementById('log');
      function setState(state, err = '') {
        switch(state) {
        case 'pending':
          logDiv.innerText = 'Requesting...';
          logDiv.className ='state-pending';
          break;
        case 'ok':
          logDiv.innerText = 'Success';
          logDiv.className ='state-ok';
          break;
        case 'error':
          logDiv.innerText = 'Error: ' + err;
          logDiv.className ='state-error';
          break;
        default:
          logDiv.innerText = '';
          logDiv.className ='';
        }
      }
      async function sendReq(url) {
        try {
          setState('pending');
          const res = await fetch(url, { method: 'POST' });
          console.log('Request complete:', res);
          if (res.ok) {
            setState('ok');
          } else {
            setState('error', res.statusText);
          }
        } catch (error) {
          console.error('Request failed:', error);
          setState('error', error.message);
        }
      }
      async function pressPower() {
        // alert('alarma');
        const res = await sendReq('/power?push=short');
      }
      async function pressPowerLong() {
        if (confirm('Are you sure to issue a long press? This will force shutdown the computer!')) {
          const ok = await sendReq('/power?push=long');
        }
      }
    </script>
  </body>
</html>
)=====";

#endif