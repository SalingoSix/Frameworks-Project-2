require "math"
CmdID, ObjectID, moveToX, moveToY, moveToZ, requiredTime, startTime, stopTime, initX, initY, initZ = getParams()
r, x, y, z, vx, vy, vz = getObjectState(ObjectID)

finished = false

travelLength = math.sqrt((moveToX - initX)^2 + (moveToY - initY)^2 + (moveToZ - initZ)^2)

constTime = requiredTime - (startTime + stopTime)

maxVel = travelLength / ((startTime / 2) + constTime + (stopTime / 2))

accel = maxVel / startTime

decel = -maxVel / stopTime

accelDist = (accel * (startTime^2)) / 2

constDist = constTime * maxVel

decelDist = travelLength - (accelDist + constDist)

normalX = (moveToX - initX) / travelLength
normalY = (moveToY - initY) / travelLength
normalZ = (moveToZ - initZ) / travelLength

distTravelled = math.sqrt((x - initX)^2 + (y - initY)^2 + (z - initZ)^2)

if (distTravelled >= travelLength) then
	vx = 0
	vy = 0
	vz = 0
	finished = true
elseif ( distTravelled < accelDist ) then
	vx = vx + (accel * normalX) * r
	vy = vy + (accel * normalY) * r
	vz = vz + (accel * normalZ) * r
elseif ( distTravelled >= (travelLength - decelDist) ) then
	vx = vx + (decel * normalX) * r
	vy = vy + (decel * normalY) * r
	vz = vz + (decel * normalZ) * r
end

totalVel = math.sqrt( vx^2 + vy^2 + vz^2 )

if ( totalVel == 0 ) then
	finished = true
end

if (normalX > 0) then
	if (vx < 0) then
		vx = 0
		vy = 0
		vz = 0
		finished = true
	end
elseif (normalX < 0) then
		if (vx > 0) then
		vx = 0
		vy = 0
		vz = 0
		finished = true
	end
end

if (normalY > 0) then
	if (vy < 0) then
		vx = 0
		vy = 0
		vz = 0
		finished = true
	end
elseif (normalY < 0) then
		if (vy > 0) then
		vx = 0
		vy = 0
		vz = 0
		finished = true
	end
end

if (normalZ > 0) then
	if (vz < 0) then
		vx = 0
		vy = 0
		vz = 0
		finished = true
	end
elseif (normalZ < 0) then
		if (vz > 0) then
		vx = 0
		vy = 0
		vz = 0
		finished = true
	end
end

x = x + (vx * r)
y = y + (vy * r)
z = z + (vz * r)

setObjectState(CmdID, ObjectID, x, y, z, vx, vy, vz, finished)