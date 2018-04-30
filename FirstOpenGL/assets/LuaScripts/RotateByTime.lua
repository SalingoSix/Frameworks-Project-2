require "math"
CmdID, ObjectID, moveToX, moveToY, moveToZ, requiredTime, startTime, stopTime, initX, initY, initZ = getParamsRotatation()
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

constTime = requiredTime - (startTime + stopTime)

maxVel = travelLength / ((startTime / 2) + constTime + (stopTime / 2))

accel = maxVel / startTime

decel = -maxVel / stopTime

accelDist = (accel * (startTime^2)) / 2

constDist = constTime * maxVel

decelDist = travelLength - (accelDist + constDist)

normalX = radiansX / travelLength
normalY = radiansY / travelLength
normalZ = radiansZ / travelLength

if (distTravelled >= travelLength) then
	finished = true
	vx = 0
	vy = 0
	vz = 0
elseif ( distTravelled < math.abs(accelDist) ) then
	vx = vx + (accel * normalX) * r
	vy = vy + (accel * normalY) * r
	vz = vz + (accel * normalZ) * r
elseif ( distTravelled > (travelLength - math.abs(decelDist)) ) then
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