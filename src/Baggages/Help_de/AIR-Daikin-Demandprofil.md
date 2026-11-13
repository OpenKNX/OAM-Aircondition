### Modellgeprüftes Demand-Profil

Nur aktivieren, wenn Minimum und Schrittweite für das konkrete Klimagerät
geprüft wurden. Ein lesbarer F7-Status und ein ACK bestätigen diese Werte nicht.
Die werkseitige Einstellung bleibt deaktiviert; das Minimum 100 % erlaubt keine
Leistungsreduzierung. Das Profil wird durch ETS gespeichert und beim Start geladen.

Demand begrenzt die angeforderte Konditionierungsleistung, nicht linear die
Leistungsaufnahme in Watt. Die Werte werden von 100 % abwärts in der angegebenen
Schrittweite gebildet. `(100 - Minimum)` muss durch die Schrittweite teilbar sein.

Faikout bietet 30–100 % in 5-%-Schritten an. Die ältere Daikin-App kennt die
Vorgaben 40/50/60 %. Das sind Bedienoberflächenwerte und keine bestätigte
Modellfreigabe. Ein unbekanntes Modell erhält deshalb kein solches Profil
automatisch. Der Rückgabewert 99 % wird wegen bekannter Mehrdeutigkeit nicht
als gültige Demand-Rückmeldung verwendet.

Die Konsole zeigt mit `s21 profiles` Modellkennung, konfigurierte Werte und
feldbezogene Schreibbestätigung. Eine erfolgreiche Rückmeldung bestätigt nur
den tatsächlich geschriebenen Wert, nicht den gesamten Bereich.
