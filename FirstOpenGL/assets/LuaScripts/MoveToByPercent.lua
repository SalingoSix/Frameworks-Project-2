require "math"
CmdID, ObjectID, moveToX, moveToY, moveToZ, maxVel, maxSpeedBy, slowDownAt, initX, initY, initZ = getParams()
r, x, y, z, vx, vy, vz = getObjectState(ObjectID)

finished = false

travelLength = math.sqrt((moveToX - initX)^2 + (moveToY - initY)^2 + (moveToZ - initZ)^2)

normalX = (moveToX - initX) / travelLength
normalY = (moveToY - initY) / travelLength
normalZ = (moveToZ - initZ) / travelLength

maxSpeedDist = travelLength * (maxSpeedBy / 100)

accel = maxVel^2 / (maxSpeedDist * 2)

startDecel = travelLength * (slowDownAt / 100)
decel = -(maxVel^2) / (startDecel * 2)

distTravelled = math.sqrt((x - initX)^2 + (y - initY)^2 + (z - initZ)^2)

percentTravelled = distTravelled / travelLength

if ( distTravelled >= travelLength ) then
	vx = 0
	vy = 0
	vz = 0
	finished = true
elseif ( (percentTravelled * 100) < maxSpeedBy ) then
	vx = vx + (accel * normalX) * r
	vy = vy + (accel * normalY) * r
	vz = vz + (accel * normalZ) * r
elseif ( (percentTravelled * 100) > (100 - slowDownAt)) then
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