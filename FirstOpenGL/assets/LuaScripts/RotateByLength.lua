require "math"
CmdID, ObjectID, moveToX, moveToY, moveToZ, maxVel, maxSpeedBy, slowDownAt, initX, initY, initZ = getParamsRotatation()
r, x, y, z, vx, vy, vz = getObjectStateRotation(ObjectID)

finished = false

radiansX = (math.pi / 180) * moveToX
radiansY = (math.pi / 180) * moveToY
radiansZ = (math.pi / 180) * moveToZ

absX = math.abs(radiansX)
absY = math.abs(radiansY)
absZ = math.abs(radiansZ)

travelLength = absX
distTravelled = math.abs(x - initX)

if ( absY > travelLength ) then
	travelLength = absY
	distTravelled = math.abs(y - initY)
end

if ( absZ > travelLength ) then
	travelLength = absZ
	distTravelled = math.abs(z - initZ)
end

normalX = radiansX / travelLength
normalY = radiansY / travelLength
normalZ = radiansZ / travelLength

maxSpeedDist = maxSpeedBy

accel = maxVel^2 / (maxSpeedDist * 2)

startDecel = slowDownAt
decel = -(maxVel^2) / (startDecel * 2)

percentTravelled = distTravelled / travelLength

if ( distTravelled >= travelLength ) then
	vx = 0
	vy = 0
	vz = 0
	finished = true
elseif ( distTravelled < maxSpeedDist ) then
	vx = vx + (accel * normalX) * r
	vy = vy + (accel * normalY) * r
	vz = vz + (accel * normalZ) * r
elseif ( distTravelled > startDecel) then
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

setObjectStateRotation(CmdID, ObjectID, x, y, z, vx, vy, vz, finished)