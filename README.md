# vtobjplugin

Virtools Obj Plugin.

A interface plugin of Virtools which allows you to export some models as `.obj` file.

Support Virtools 3.5 and 5.0

Features:

* Support omit world matrix (comparing with `vt2obj`).
* Support rrepostion script generator (for 3ds Max and Blender).
* Support option of right-hand, left-hand coordinate system convertion.
* Support more texture export option.

Shortcomings:

* Right-left hand coordinate system convertion only support swaping Z/Y axis.
* Don't support detecting texture original format.
* Smoothing group and some weird bugs.
