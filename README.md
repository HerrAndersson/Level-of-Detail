# Level-of-Detail
The experiment for the course DV1478 Kandidatarbete i datavetenskap. Demonstrates four different methods of handling Level-of-Detail of 3d meshes, and compares the performance and image space metrics.

Methods compared:

-Popping switching between static meshes
-Unpopping blended switching between static meshes
-Generating more geometry through hardware tessellation over the whole mesh
-Generating more geometry through hardware tessellation only on the silhouette of the mesh


The purpose is to examine how the silhouette-based version compares to the other methods, and to see if it is a valid candidate to use in games.
