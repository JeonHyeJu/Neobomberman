#pragma once

enum class EBombMatrixState
{
	Empty = 0,
	Exist,
	Explosion
};


class SBombMatrix
{
public:
	SBombMatrix() {}
	SBombMatrix(const FIntPoint& _shape, const FVector2D& _elemSize)
	{
		Init(_shape, _elemSize);
	}

	void Init(const FIntPoint& _shape, const FVector2D& _elemSize)
	{
		Shape = _shape;
		ElemSize = _elemSize;

		Matrix.resize(_shape.Y);
		for (size_t y = 0, rows = _shape.Y; y < rows; y++)
		{
			Matrix[y].resize(_shape.X);

			for (size_t x = 0, cols = _shape.X; x < cols; x++)
			{
				Matrix[y][x] = EBombMatrixState::Empty;
			}
		}
	}

	bool IsIndexOver(const FIntPoint& _idx)
	{
		if (0 > _idx.X)
		{
			return true;
		}

		if (0 > _idx.Y)
		{
			return true;
		}

		if (Shape.X - 1 < _idx.X)
		{
			return true;
		}

		if (Shape.Y - 1 < _idx.Y)
		{
			return true;
		}

		return false;
	}

	EBombMatrixState GetValue(const FIntPoint& _idx)
	{
		if (IsIndexOver(_idx) == true)
		{
			return EBombMatrixState::Empty;
		}

		return Matrix[_idx.Y][_idx.X];
	}

private:
	FIntPoint Shape;
	FVector2D ElemSize;
	std::vector<std::vector<EBombMatrixState>> Matrix;
};
