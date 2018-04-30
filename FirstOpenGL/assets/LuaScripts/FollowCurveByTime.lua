require "math"
CmdID, ObjectID, endPointX, endPointY, endPointZ, midPointX, midPointY, midPointZ, midPoint2X, midPoint2Y, midPoint2Z, requiredTime, timeSoFar, initX, initY, initZ = getParamsBezier()
r, x, y, z = getObjectStateBezier(ObjectID)

finished = false

timeSoFar = timeSoFar + r

reducedTimeSoFar = timeSoFar / requiredTime

if (timeSoFar > requiredTime) then
	reducedTimeSoFar = 1
	finished = true
end

x = (1 - reducedTimeSoFar)^3 * initX + (3 * reducedTimeSoFar * (1 - reducedTimeSoFar)^2) * midPointX + (3 * reducedTimeSoFar^2 * (1 - reducedTimeSoFar)) * midPoint2X + reducedTimeSoFar^3 * endPointX

y = (1 - reducedTimeSoFar)^3 * initY + (3 * reducedTimeSoFar * (1 - reducedTimeSoFar)^2) * midPointY + (3 * reducedTimeSoFar^2 * (1 - reducedTimeSoFar)) * midPoint2Y + reducedTimeSoFar^3 * endPointY

z = (1 - reducedTimeSoFar)^3 * initZ + (3 * reducedTimeSoFar * (1 - reducedTimeSoFar)^2) * midPointZ + (3 * reducedTimeSoFar^2 * (1 - reducedTimeSoFar)) * midPoint2Z + reducedTimeSoFar^3 * endPointZ

setObjectStateBezier(CmdID, ObjectID, x, y, z, timeSoFar, finished)