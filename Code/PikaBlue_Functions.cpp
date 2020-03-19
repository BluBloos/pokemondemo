/*
Hi thank you for choosing the PikaBlue engine!
This file contains all the engine functions 
that don't adhere to some other category of 
functions.
*/

//NOTE: Make sure to keep functions in this file functional. Because after all the name of this file 
//is "functions" so it better be god damned functional. Right?

//this is pretty functional. Just takes in a list and an Id and returns something
inline pokemon_move FillPokemonMove(pokemon_move *MoveDatabase, unsigned int MoveID)
{
	pokemon_move Result = {};
	pokemon_move Move = MoveDatabase[MoveID];

	//NOTe below I do a lot of cloning just a warning!
	//It's a warning because I think its dumb.
	//I think generally if you have to clone something you may be doing something wrong
	//although there are exceptions, sometimes. Maybe this is an exception. Who knows?
	Result.Effect = Move.Effect;
	Result.Type = Move.Type;
	Result.BasePower = Move.BasePower;
	Result.Accuracy = Move.Accuracy;
	Result.MaxPP = Move.PP;
	Result.PP = Move.PP;
	Result.EffectAccuracy = Move.EffectAccuracy;
	Result.AffectsWhom = Move.AffectsWhom;
	Result.Priority = Move.Priority;
	Result.Flags = Move.Flags;
	CloneString(Move.Name, Result.Name, 15);

	return Result;
}

//this function will ask if they are equal and if they are not it will check if they are equal by some 
//very stupid small amount.
inline unsigned int AreFloatsEqual(float Float1, float Float2)
{
	unsigned int Result = false;
	if (Float1 == Float2)
	{
		Result = true;
	}
	else if ( Abs(Float1 - Float2) < Pow(10.0f,-16.0f) )
	{
		Result = true;
	}
	return Result;
}

//will take ASCII and shoot back the official pokemon demo font
internal loaded_bitmap BitmapFromASCII(loaded_bitmap *PokemonDemoFont, char Character)
{
	loaded_bitmap Result = {};
	unsigned int Index = (unsigned int) Character;

	if ( (Character >= 65) && (Character <= 90) )
	{
		Index -= 65;
	}
	else if ( (Character >= 97) && (Character <= 122) )
	{
		Index -= 67;	
	}
	else if ( (Character >= 48) && (Character <= 57) )
	{
		Index += 12;
	}
	else if (Character == '!')
	{
		Index = 26;
	}
	else if (Character == '.')
	{
		Index = 27;
	}
	else if (Character == '?')
	{
		Index = 28;
	}
	else if ( Character == ',' || Character == 39 ) 
	{
		Index = 29;
	}
	else
	{
		Index = 0;
	}

	Result = PokemonDemoFont[Index];
	return Result;
}

internal loaded_bitmap GetSpriteByRect(memory_arena *WorldArena, loaded_bitmap *SpriteMap, unsigned int *CharacterPointer, int Width, int Height,
	unsigned int BackgroundColor)
{
	loaded_bitmap Result = {};
	Result.Scale = MASTER_BITMAP_SCALE;

	if ( !(Width < 0) )
	{
		Result.Width = Width;
		Result.Height = Height;
		Result.PixelPointer = PushArray(WorldArena, (Result.Width * Result.Height), unsigned int);

		unsigned int *Row = CharacterPointer;
		unsigned int *DestRow = Result.PixelPointer + Result.Width * (Result.Height - 1);

		for (unsigned int Y = 0; Y < Result.Height; Y++)
		{	
			unsigned int *Pixel = (unsigned int *)Row;
			unsigned int *DestPixel = (unsigned int *)DestRow;
			for (unsigned int X = 0; X < Result.Width; X++)
			{
				if ( (BackgroundColor) && (*Pixel == BackgroundColor) ) 
				{
					*DestPixel = 0x00000000;
				}
				else
				{
					*DestPixel = *Pixel | 0xFF000000;	
				}
				DestPixel++;
				Pixel++;
			}
			DestRow -= Result.Width;
			Row -= SpriteMap->Width;
		}
	}
	else
	{
		//There is no sprite here just background
		//so just return a fully black sprite.
		Result.Width = 16;
		Result.Height = 16;
		Result.PixelPointer = PushArray(WorldArena, Result.Width * Result.Height, unsigned int);
	}

	return Result;
}

internal loaded_bitmap GetSpriteFromSpriteMap(memory_arena *WorldArena,
	loaded_bitmap *SpriteMap, unsigned int *PixelPointer, unsigned int TileSize)
{
	unsigned int MinX = TileSize;
	unsigned int MaxX = 0;
	unsigned int MinY = TileSize;
	unsigned int MaxY = 0;

	unsigned int *Row = PixelPointer;
	unsigned int BackgroundColor = *Row;

	for (unsigned int Y = 0; Y < TileSize;++Y)
	{	
		unsigned int *Pixel = (unsigned int *)Row;
		for (unsigned int X = 0; X < TileSize; ++X)
		{
			if (*Pixel != BackgroundColor)
			{
				//so I found something that actually has valid data.
				if (X < MinX)
				{
					MinX = X;
				}
				if (X > MaxX)
				{
					MaxX = X;
				}
				if (Y < MinY)
				{
					MinY = Y;
				}
				if (Y > MaxY)					
				{
					MaxY = Y;
				}
			}
			Pixel++;
		}
		Row -= SpriteMap->Width;
	}

	unsigned int *NewRow = PixelPointer - MinY * SpriteMap->Width + MinX;
	return GetSpriteByRect(WorldArena, SpriteMap, NewRow, 1 + MaxX - MinX, 1 + MaxY - MinY, BackgroundColor);
}

internal void GenerateByTiles(memory_arena *MemoryArena, loaded_bitmap Bitmap, unsigned int TileWidth, 
	unsigned int TileHeight, loaded_bitmap *Dest)
{
	unsigned int BitmapTileWidth = Bitmap.Width / TileWidth;
	unsigned int BitmapTileHeight = Bitmap.Height / TileHeight;
	Assert( !(BitmapTileWidth * TileWidth > Bitmap.Width) );
	Assert( !(BitmapTileHeight * TileHeight > Bitmap.Height) );
	unsigned int *Row = Bitmap.PixelPointer + Bitmap.Width * (Bitmap.Height - 1);
	
	for (unsigned int y = 0; y < BitmapTileHeight; y++)
	{
		unsigned int *Block = Row;
		for (unsigned int x = 0; x < BitmapTileWidth; x++)
		{
			Dest[y * BitmapTileWidth +x] = GetSpriteFromSpriteMap(MemoryArena, &Bitmap, Block, TileWidth);
			Block += TileWidth;
		}
		Row -= Bitmap.Width * TileHeight;
	}
}

internal void UnPackBitmapTiles(memory_arena *WorldArena, loaded_bitmap Bitmap,
	unsigned int TileWidth, unsigned int TileHeight, unsigned int TilePadding, loaded_bitmap *Dest)
{
	unsigned int BitmapTileWidth = (Bitmap.Width + TilePadding) / (TileWidth + TilePadding);
	unsigned int BitmapTileHeight = (Bitmap.Height + TilePadding) / (TileHeight + TilePadding);
	Assert( !(BitmapTileWidth * TileWidth > Bitmap.Width) );
	Assert( !(BitmapTileHeight * TileHeight > Bitmap.Height) );
	unsigned int *Row = Bitmap.PixelPointer + Bitmap.Width * (Bitmap.Height - 1);
	
	for (unsigned int y = 0; y < BitmapTileHeight; y++)
	{
		unsigned int *Block = Row;
		for (unsigned int x = 0; x < BitmapTileWidth; x++)
		{
			Dest[y * BitmapTileWidth +x] = GetSpriteByRect(WorldArena, &Bitmap, Block, TileWidth, TileHeight, 0xFFFFFFFF);
			Block += TileWidth + TilePadding;
		}
		Row -= Bitmap.Width * (TileHeight + TilePadding);
	}
}

internal unsigned int ScreenPositionInRect(game_screen_position ScreenPos, rect Rect)
{
	unsigned int Result = false;	
	if ( (ScreenPos.X > Rect.MinX) && (ScreenPos.X < Rect.MaxX) && (ScreenPos.Y > Rect.MinY) && (ScreenPos.Y < Rect.MaxY) )
	{
		Result = true;
	}
	return Result;
}

internal float Dot(vector2f v1, vector2f v2)
{
	return v1.X * v2.X + v1.Y * v2.Y;
}

internal float LineLength(vector2f p1, vector2f p2)
{
	return Pow(Pow(p2.X - p1.X, 2.0f) + Pow(p2.Y - p1.Y, 2.0f), 0.5f);
}

internal vector2f Normalized(vector2f v)
{
	vector2f origin = {};
	float Mag = LineLength(origin, v);
	v.X = v.X / Mag;
	v.Y = v.Y / Mag;
	return v;
}

internal hf_entity *GetEntityAhead(game_state *GameState, hf_entity Player, float TileSizeInMeters)
{
	//so what we are going to do is determine the tile right in front of us
	//then check if any entity out of all the entities collide with that tile
	//if they do return that entity
	hf_entity *Result = {};

	for (unsigned int x = 0; x < GameState->EntityCount; x++)
	{
		if (GameState->EntityResidence[x] == ENTITY_HIGH)
		{
			hf_entity Entity = GameState->HighEntities[x];
			
			vector2f PlayerPos = {}; vector2f InteractablePos = {}; vector2f PlayerDirection = {};
			PlayerPos.X = Player.Pos.X; PlayerPos.Y = Player.Pos.Y; 
			InteractablePos.X = Entity.Pos.X; InteractablePos.Y = Entity.Pos.Y;

			//below we shift both positions such that the player is at the origin
			InteractablePos.X -= PlayerPos.X; PlayerPos.X = 0.0f;
			InteractablePos.Y -= PlayerPos.Y; PlayerPos.Y = 0.0f;

			if (Player.MoveDirection == UP)
			{
				PlayerDirection.X = 0.0f; PlayerDirection.Y = 1.0f; 
			}
			else if (Player.MoveDirection == DOWN)
			{
				PlayerDirection.X = 0.0f; PlayerDirection.Y = -1.0f;
			}
			else if (Player.MoveDirection == LEFT)
			{
				PlayerDirection.X = -1.0f; PlayerDirection.Y = 0.0f;
			}
			else if (Player.MoveDirection == RIGHT)
			{
				PlayerDirection.X = 1.0f; PlayerDirection.Y = 0.0f;
			}

			if ( (LineLength(PlayerPos, InteractablePos) < TileSizeInMeters * 1.5f) && 
				(Dot(PlayerDirection, Normalized(InteractablePos)) > 0.5f) )
			{
				Result = &GameState->HighEntities[x];
				break;
			}
		}
	}
	
	return Result; 
} 

internal entity_npc *GetNPCFromEntity(entity_npc *Testificates, entity *Entity)
{
	entity_npc *Result = {};
	for (unsigned int x = 0; x < TESTIFICATE_AMOUNT; x++)
	{
		entity_npc NPC = Testificates[x];
		if (NPC.Entity == Entity)
		{
			Result = &Testificates[x];
			break;
		}
	}
	return Result;
}