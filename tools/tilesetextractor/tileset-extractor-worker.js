self.onmessage = function ( event )
{
	var data = event.data;

	if( data.action == "extract" )
		extract( data.imageData, data.tileWidth, data.tileHeight, data.tolerance );
};

function sendStart()
{
	self.postMessage( {action: "extract-start"} );
}

function sendProgress( progress )
{
	self.postMessage( {
		action: "extract-progress",
		progress: progress
	} );
}

function sendResult( tiles, map, startTime )
{
	self.postMessage( {
				action: "extract-result",
				tiles: tiles,
				map: map,
				time: new Date().getTime() - startTime
			}
	);
}

function extract( imageData, tileWidth, tileHeight, tolerance )
{
	sendStart();

	var startTime = new Date().getTime();

	var sourceWidth = imageData.width;
	var sourceHeight = imageData.height;
	var sourceArray = imageData.data;

	function createTileFrom()
	{
		var tileData = new ImageData( tileWidth, tileHeight );
		var deltaX = tileX * tileWidth;
		var deltaY = tileY * tileHeight;
		var tileArray = tileData.data;
		var tileIndex = 0;

		for( var y = 0; y < tileHeight; ++y )
		{
			for( var x = 0; x < tileWidth; ++x )
			{
				var sourceIndex = ((deltaY + y) * sourceWidth + (deltaX + x)) << 2;

				for( var i = 0; i < 4; ++i )
					tileArray[tileIndex++] = sourceArray[sourceIndex++];
			}
		}
		return tileData;
	}

	function compareTileWith( tileX, tileY, tile )
	{
		var deltaX = tileX * tileWidth;
		var deltaY = tileY * tileHeight;

		var targetIndex = 0;
		var difference = 0;

		for( var y = 0; y < tileHeight; ++y )
		{
			for( var x = 0; x < tileWidth; ++x )
			{
				var sourceIndex = ((deltaY + y) * sourceWidth + (deltaX + x)) << 2;

				for( var i = 0; i < 4; ++i )
					difference += Math.abs( tile[targetIndex++] - sourceArray[sourceIndex++] );

				if( tolerance < difference )
					return false;
			}
		}
		return true;
	}

	var numCols = (sourceWidth / tileWidth) | 0;
	var numRows = (sourceHeight / tileHeight) | 0;
	var numTiles = numCols * numRows;
	var tiles = [];
	var map = [];
	var index;

	for( var tileIndex = 0; tileIndex < numTiles; ++tileIndex )
	{
		var tileX = (tileIndex % numCols) | 0;
		var tileY = (tileIndex / numCols) | 0;

		var tileExist = false;

		for( index = 0; index < tiles.length; ++index )
		{
			if( compareTileWith( tileX, tileY, tiles[index].data ) )
			{
				tileExist = true;
				break;
			}
		}
		if( !tileExist )
		{
			tiles.push( createTileFrom() );
		}

		map.push( index );

		if( tileIndex % 32 == 0 )
		{
			sendProgress( tileIndex / numTiles );
		}
	}
	sendResult( tiles, map, startTime );
}