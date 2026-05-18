function AIR_toText(resp, offset) {
  var text = "";
  for (var i = offset; i < resp.length; i++) {
    if (resp[i] === 0) break;
    text += String.fromCharCode(resp[i]);
  }
  return text;
}

function AIR_errorText(code) {
  switch (code) {
    case 1:
      return "Live-Daten sind für den gewählten Treiber noch nicht verfügbar.";
    case 2:
      return "Aktiver Treiber unterstützt Live-Daten nicht.";
    case 3:
      return "Keine Antwort des Klimageräts innerhalb des Timeouts.";
    case 4:
      return "Unbekannte Online-Funktion.";
    case 5:
      return "Ungültige Online-Anfrage.";
    default:
      return "Unbekannter Fehlercode " + code + ".";
  }
}

function AIR_readTextChunks(online, command, totalLength, progress, label) {
  var text = "";
  var offset = 0;
  var more = true;

  while (more && offset < totalLength) {
    var request = [command, (offset >> 8) & 0xff, offset & 0xff];
    var resp = online.invokeFunctionProperty(160, 3, request);

    if (!resp || resp.length < 2 || resp[0] !== 0) {
      throw new Error(label + ": " + AIR_errorText(resp && resp.length > 0 ? resp[0] : 255));
    }

    more = resp[1] !== 0;
    text += AIR_toText(resp, 2);
    offset = text.length;

    if (totalLength > 0) {
      var percent = Math.min(95, Math.round((offset * 100) / totalLength));
      progress.setProgress(percent);
    }
  }

  return text;
}

function AIR_readLiveData(device, online, progress, context) {
  var parRawDump = device.getParameterByName("AIR_LiveRawDump");
  var parDecodedDump = device.getParameterByName("AIR_LiveDecodedDump");

  parRawDump.value = "";
  parDecodedDump.value = "";

  progress.setText("Live-Daten auslesen...");
  progress.setProgress(0);

  try {
    online.connect();

    var resp = online.invokeFunctionProperty(160, 3, [1]);
    if (!resp || resp.length < 5 || resp[0] !== 0) {
      throw new Error(AIR_errorText(resp && resp.length > 0 ? resp[0] : 255));
    }

    var rawLength = (resp[1] << 8) | resp[2];
    var decodedLength = (resp[3] << 8) | resp[4];

    progress.setText("Rohdump lesen...");
    parRawDump.value = AIR_readTextChunks(online, 2, rawLength, progress, "Rohdump");

    progress.setText("Daten dekodieren...");
    parDecodedDump.value = AIR_readTextChunks(online, 3, decodedLength, progress, "Dekodierung");

    online.disconnect();
    progress.setProgress(100);
    progress.setText("Live-Daten ausgelesen.");
  } catch (e) {
    try {
      online.disconnect();
    } catch (ignore) {
    }
    throw new Error("Live-Daten: " + e.message);
  }
}
