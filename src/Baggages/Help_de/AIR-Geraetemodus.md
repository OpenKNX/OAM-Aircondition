### Gerätemodus

Optionen für Daikin:

- **Standard**: Normaler Betriebsmodus
- **Hi-Power**: Entspricht Daikin "Powerful"-Modus für maximale Leistung
- **ECO**: Econo über D7/F7. Der Gerätemodus-/Szenen-Preset schaltet Powerful und Quiet ab und wartet auf die passende Rückmeldung, bevor Econo gesetzt wird. Ein negatives FU00-Flag allein sperrt Econo nicht.
- **Leise 1**: Daikin "Quiet". S21 bietet dafür genau einen Ein/Aus-Zustand.
- **Leise 2 (Legacy-Gruppenobjekte)**: Kompatibilitätsalias für Leise 1, keine zweite Geräuschstufe. Die Rückmeldung erfolgt als Leise 1; Leise-2-Status bleibt 0. Daikin-Szenen bieten diese Alias-Auswahl nicht an.

In Szenen steht zusätzlich „Keine Änderung“ zur Verfügung.

Über die unabhängigen Objekte können Econo und Außengerät Quiet gemeinsam angefordert werden. Powerful schließt beide aus; der einzelne Gerätemodus-/Szenen-Preset wählt dagegen jeweils einen Power-Preset.

Ein noch nicht schreibbestätigter unabhängiger Econo-Ein-Auftrag wird bei aktivem Powerful zurückgewiesen. Für diesen ersten Einzelversuch Powerful vorher ausschalten; die ausdrückliche Preset-Wahl ECO führt den Wechsel selbst aus. Die tatsächliche Ausführung bleibt modell- und betriebsmodusabhängig.

Die unabhängigen Optionen melden ihre Lesbarkeit/Verfügbarkeit im Bitmaskenobjekt „Optionen abfragbar“ (Econo: Bit 1). Lesbarkeit und Verfügbarkeit bestätigen noch keinen erfolgreichen Schreibzugriff.

Optionen für Toshiba:

- Keine Änderung
- Standard
- Hi-Power
- ECO
- Außengerät Leise 1
- Außengerät Leise 2

Für Mitsubishi steht diese Option nicht zur Verfügung.
