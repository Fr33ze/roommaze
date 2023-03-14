# Roommaze

Das Spiel RoomMaze wird durch Maximilian Kinzel und Lukas Lidauer entwickelt. Anfänglich
wurde das OpenGL Framework erstellt, wo simple Geometrien mit verschiedenen
Texturen/Maps erstellt werden konnten und die Kamera frei im Raum bewegt werden
konnte. Das Programm wurde um einen Wavefront Object Reader erweitert, da wir nicht
von der Funktionalität von Assimp abhängig sein wollten. Die Funktionalität unseres
Frameworks und des Object Reader ist laufend um diverse Maps gestiegen. Als das
Grund-Framework stand, wurde PhysX in das Projekt eingebunden und die Klassenstruktur
dementsprechend etwas verändert. Zeitgleich wurde auch das HUD entwickelt
(Implementierung des Font-Frameworks Freetype). Später wurde noch für den 3D Audio
OpenAL eingebaut. Über den gesamten Zeitraum dieser LVA wurde ein Model in Blender
entworfen woran wir insgesamt etwa 300 Stunden saßen. Alle im Spiel befindlichen Models
sind eigenständig entwickelt worden.

<img src="https://user-images.githubusercontent.com/2223159/224984953-f37c35cf-a03f-4ac6-917e-6dcf71a7b1d3.png" width="600">

## Gruppeninformation
Vorname | Nachname | Matrikelnummer | Studienkennzahl | E-Mail
--- | --- | --- | --- | ---
Lukas | Lidauer | 01635862 | 033 532 | lukas.lidauer@tuwien.ac.at
Maximilian |	Kinzel |	01526044 |	033 532 | e1526044@student.tuwien.ac.at

## How to start
Vor dem ersten Start auf einem PC bitte unbedingt OpenAL 1.1 installieren, indem die
mitgelieferte Datei oalinst.exe ausgeführt und installiert wird. Einstellungen können über die
Datei assets/settings.ini gesetzt werden.

## Features (Gameplay)
- Playable: Das Spiel ist gänzlich spielbar. Es gibt keine Möglichkeit, im Spiel hängen zu
bleiben.
- 3D Geometry: Es wurde eigene 3D Geometrie modelliert und in das Spiel importiert.
- Win/Loose Condition: Wenn man 5 Sekunden im Dunkeln wandert, wird man von
einem Geist attackiert. Wenn man es schafft mit dem Aufzug an die Oberfläche zu
kommen, ist das Spiel gewonnen.
- Intuitive Controls: Es gibt verschiedene Controls für das Bewegen (WASD), Laufen mit
gedrückter Shift-Taste, Interagieren (Linke Maustaste), Alle Items erhalten (F10),
Glow Effekt ein-/ausschalten (B), Batterie einlegen (Leertaste).
- Intuitive Camera: Die Kamera bewegt sich abhängig von der Maus und von der
Bewegung des Spielers.
- Textures: Es gibt viele verschiedene Texturen mit vielen verschiedenen Maps
(Specular, Normal, Alpha, Diffuse)
- Moving Objects: Türen, Aufzug, Schlüssel, Aufzugsknopf werden mit einem eigenen
KeyFrame system linear animiert. Diese Animationen sind aktuell noch hardcoded.
- Adjustable Parameters: In der Datei „assets/settings.ini“ können alle gewünschten
Einstellungen vorgenommen werden.
- Physics Engine: Es wurde PhysX 4.1 eingebunden und auch für viele Dinge benutzt
(Raycast, Collision Detection, ..)
- Heads-Up Display: Zeigt Start, Ende und Gameover Screen an. Außerdem wird
während des Spiels der Batteriestatus, das Inventar und ein Infotext des Fokusierten
Gegenstandes angezeigt.

## Features (Programming)
- CPU Particle System: Das Partikel-System wurde nach dem Tutorial auf
learnopengl.com implementiert. Partikel werden außerhalb des Shaders erstellt und
simuliert und anschließend gerendert unter Verwendung von Instancing. Beim
Interagieren mit Objekten werden Partikel gespawnt sowie im ersten Raum in einem
Schaltkasten.
- Hierarchical Animation: Für die Animationen haben wir ein eigenes Keyframe-System
entworfen, welches zwischen einzelnen Positionen interpoliert.
- Specular Map: Wurden wie üblich im Fragment-Shader anstatt einer Specular Color
mit einberechnet.
- Simple Normal Mapping: Für diesen Effekt haben wir die Tangenten beim Laden der
Meshes errechnet und diese anschließend in den Shader geladen. Dort wurde diese
Information zusammen mit der jeweiligen Normal Map verwendet um eine
realistische Licht-Simulation nach zu ahmen.
- Bloom/Glow: Dieser Effekt wurde nach einem Tutorial eines Youtubers entwickelt
(ThinMatrix). Hierfür wurden Framebuffer verwendet. Zuerst wird die gesamte Szene
auf einen Framebuffer gerendert. Der zugehörige "phong.frag"-Shader liefert 2
Outputs, einmal das üblich gerenderte Bild und ein Bild was sehr helle Bereiche
enthält. Dieses zweite Bild wird durch einen Gauß-Filter bearbeitet und anschließend
mit dem ersten Output kombiniert. Ein Beispiel für diesen Effekt befindet sich im Lift
(später mehr dazu).

## Additional Libraries
PhysX 4.1: https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Index.html

FreeType: https://www.freetype.org/

OpenAL 1.1: https://www.openal.org/downloads/

FreeALUT: https://github.com/vancegroup/freealut

## 4. Walkthrough
[![YOUTUBE](https://user-images.githubusercontent.com/2223159/224563567-3b27facf-7d50-4258-8bc0-7f5e3c8b1a6f.svg)](https://youtu.be/p_cL7sfWz5g)
