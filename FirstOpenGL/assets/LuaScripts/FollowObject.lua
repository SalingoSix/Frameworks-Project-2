require "math"
CmdID, ObjectID, FollowID, closestDistance, furthestDistance, maxFollowSpeed = getParamsFollow()
r, x, y, z, vx, vy, vz, Fx, Fy, Fz = getObjectStateFollow(ObjectID, FollowID)

distFromFollow = math.sqrt((Fx - x)^2 + (Fy - y)^2 + (Fz - z)^2)

finished = false

normalX = (Fx - x) / distFromFollow
normalY = (Fy - y) / distFromFollow
normalZ = (Fz - z) / distFromFollow

arbitraryTimeValue = 3

accel = maxFollowSpeed / arbitraryTimeValue

distanceRange = furthestDistance - closestDistance

adjustedDistFromFollow = distFromFollow - closestDistance

if (distFromFollow > furthestDistance) then
	vx = vx + (accel * normalX) * r
	vy = vy + (accel * normalY) * r
	vz = vz + (accel * normalZ) * r
	
	totalVel = math.sqrt(vx^2 + vy^2 + vz^2)
	
	if (totalVel > maxFollowSpeed) then
		vx = maxFollowSpeed * normalX
		vy = maxFollowSpeed * normalY
		vz = maxFollowSpeed * normalZ
	end
	
elseif (distFromFollow > closestDistance) then
	ratio = adjustedDistFromFollow / distanceRange
	newFollowSpeed = maxFollowSpeed * ratio
	
	vx = newFollowSpeed * normalX
	vy = newFollowSpeed * normalY
	vz = newFollowSpeed * normalZ
	
elseif (distFromFollow <= closestDistance) then
	vx = 0
	vy = 0
	vz = 0
	
end

x = x + (vx * r)
y = y + (vy * r)
z = z + (vz * r)

setObjectState(CmdID, ObjectID, x, y, z, vx, vy, vz)