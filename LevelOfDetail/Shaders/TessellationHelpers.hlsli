/*Returns a distance adaptive tessellation scale factor (0.0f -> 1.0f)
minDistance - Minimum distance that maximum tessellation factors should be applied at
range       - Range beyond the minimum distance where tessellation will scale down to the minimum scaling factor
*/
float GetDistanceAdaptiveScaleFactor(float3 cameraPos, float3 edgePos0, float3 edgePos1, float minDistance, float range)
{
	float3 midPoint = (edgePos0 + edgePos1) * 0.5f;
	float dist = distance(midPoint, cameraPos) - minDistance;
	float scale = 1.0f - saturate(dist / range);

	return scale;
}

/* Returns the orientation adaptive tessellation factor (0.0f -> 1.0f)
edgeDotProduct    - Dot product of edge normal with view vector
silhouetteEpsilon - Epsilon to determine the range of values considered to be silhoutte
*/
float GetOrientationAdaptiveScaleFactor(float edgeDotProduct, float silhouetteEpsilon)
{
	float scale = 1.0f - abs(edgeDotProduct);
	scale = saturate((scale - silhouetteEpsilon) / (1.0f - silhouetteEpsilon));
	return scale;
}

/* Returns the dot product between the viewing vector and the patch edge
edgeNormal0 - Normalized normal of the first control point of the given patch edge
edgeNormal1 - Normalized normal of the second control point of the given patch edge
viewVector  - Normalized viewing vector
*/
float GetEdgeDotProduct(float3 edgeNormal0, float3 edgeNormal1, float3 viewVector)
{
	float3 edgeNormal = normalize((edgeNormal0 + edgeNormal1) * 0.5f);
	float edgeDot = dot(edgeNormal, viewVector);
	return edgeDot;
}


/*Returns back face culling test result (true / false)
edgeDotProduct0 - Dot product of edge 0 normal with view vector
edgeDotProduct1 - Dot product of edge 1 normal with view vector
edgeDotProduct2 - Dot product of edge 2 normal with view vector
backfaceEpsilon - Epsilon to determine cut off value for what is considered back facing
*/
bool BackFaceCull(float edgeDotProduct0, float edgeDotProduct1, float edgeDotProduct2, float backfaceEpsilon)
{
	float3 backfaceCull;

	backfaceCull.x = (edgeDotProduct0 > -backfaceEpsilon) ? (0.0f) : (1.0f);
	backfaceCull.y = (edgeDotProduct1 > -backfaceEpsilon) ? (0.0f) : (1.0f);
	backfaceCull.z = (edgeDotProduct2 > -backfaceEpsilon) ? (0.0f) : (1.0f);

	return all(backfaceCull);
}