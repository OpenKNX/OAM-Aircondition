### Horizontale Lamellenstellung

Daikin-Szenen bieten:

- Keine Änderung
- Halten (Code 0)
- Position 1–8

Voraussetzung sind ein lesbarer FR-Status und ein ausdrücklich konfiguriertes horizontales Modellmaximum. Werte oberhalb dieses Maximums werden abgewiesen. Für feste Stellungen Swing auf der horizontalen Achse vorher ausschalten.

Der Code 0 hält die aktuelle Position und bezeichnet weder Mitte noch einen Winkel. Die Indizes sind modellabhängig; die Anzahl unterschiedlicher Winkel muss am Gerät bestimmt werden. Eine Änderung erhält den vertikalen Code und die übrigen Registerbytes.

Das separate Code-Objekt hat Status, Gültigkeit, konfiguriertes Maximum und eine Bestätigungsmaske. Eine passende Rückmeldung bestätigt den geschriebenen Code, nicht die physische Stellung.
