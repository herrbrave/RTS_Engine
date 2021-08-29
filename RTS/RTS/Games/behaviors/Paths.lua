
-- Global Path Variables
_path = nil
_pathIndex = -1
_pathLength = 0
_pathTarget = Vector2f.new(0.0, 0.0)

function isThereAPath()
	if _path == nil or _pathLength <= 0 then
		return false
	end
	return true
end

function getEndOfPath()
	if isThereAPath() == false then
		return -1, -1
	end

	return _path[_pathLength][1], _path[_pathLength][2]
end

function constructPath(_pathRef)
	_path = {}
	_pathLength = _pathRef:size() - 1
	for _index=0,_pathLength do
		_path[_index] = {_pathRef:getX(_index), _pathRef:getY(_index)}
	end
	_pathIndex = 0
end

function updatePath(_id, _speed, _delta)
	if isThereAPath() == false then
		return
	end

	_step = _delta / 1000.0
	_dist = _speed * _step

	while _dist > 0 and _pathIndex <= _pathLength do
		_dist = moveAlongPath(_id, _dist)
	end
end


function moveAlongPath(_id, _dist)
	_pathX = _path[_pathIndex][1]
	_pathY = _path[_pathIndex][2]

	_pathEntityPosition = getPosition(_id)
	_pathTarget:setX(_pathX)
	_pathTarget:setY(_pathY)
	_pathTarget:subtract(_pathEntityPosition)

	_moveDist = _pathTarget:magnitude()

	if _moveDist < _dist then
		_pathTarget:normalize()
		_pathTarget:scale(_moveDist)
		moveAndSlide(_id, _pathTarget:getX(), _pathTarget:getY())
		_pathIndex = _pathIndex + 1
		if _pathIndex > _pathLength then
			_path = nil
			_pathLength = -1
		end

		return _dist - _moveDist
	else 
		_pathTarget:normalize()
		_pathTarget:scale(_dist)
		moveAndSlide(_id, _pathTarget:getX(), _pathTarget:getY())
	end

	_pathEntityPosition = nil
	return 0.0
end