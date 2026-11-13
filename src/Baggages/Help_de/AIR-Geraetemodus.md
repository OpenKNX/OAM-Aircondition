### Gerätemodus

Optionen für Daikin:

- **Standard**: Normaler Betriebsmodus
- **Hi-Power**: Entspricht Daikin "Powerful"-Modus für maximale Leistung
- **ECO**: Daikin "Econo" über F7. Das FU00-Flag ist ein Hinweis und kann fehlen. Unbestätigte Econo-Aufträge beenden aktives Powerful nicht automatisch; für den ersten Versuch Powerful vorher ausschalten.
- **Leise 1**: Daikin "Quiet". S21 bietet dafür genau einen Ein/Aus-Zustand.
- **Leise 2**: Kompatibilitätsalias für Leise 1, keine zweite Geräuschstufe. Die Rückmeldung erfolgt als Leise 1; Leise-2-Status bleibt 0.

Econo und Außengerät Quiet können gemeinsam aktiv sein. Powerful schließt beide aus.

Die unabhängigen Optionen melden ihre Lesbarkeit/Verfügbarkeit im Bitmaskenobjekt „Optionen abfragbar“ (Econo: Bit 1). Lesbarkeit und Verfügbarkeit bestätigen noch keinen erfolgreichen Schreibzugriff.

Optionen für Toshiba:

- Keine Änderung
- Standard
- Hi-Power
- ECO
- Außengerät Leise 1
- Außengerät Leise 2

