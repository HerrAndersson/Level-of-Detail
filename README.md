# Level-of-Detail
The experiment for the course DV1478 Kandidatarbete i datavetenskap. Demonstrates four different methods of handling Level-of-Detail of 3d meshes, and compares the performance and image space metrics.

Methods compared:

-Static LoD
-Blended switching between static meshes using Unpopping LoD
-Generating more geometry through hardware tessellation over the whole mesh using Curved PN Triangles
-Generating more geometry through hardware tessellation only on the silhouette of the mesh using Phong Tessellation


The purpose is to examine how the silhouette-based version compares to the other methods, and to see if it is a valid candidate to use in games.
