
class GameGeom {

private:
	// passed to GPU, transformed

	FIVector4 boundsMinInPixels;
	FIVector4 boundsMaxInPixels;
	FIVector4 visMinInPixels;
	FIVector4 visMaxInPixels;

public:

	// passed to GPU

	FIVector4 geomParams[E_GP_LENGTH];

	// enum E_GEOM_PARAMS {
	// E_GP_BOUNDSMININPIXELST,
	// E_GP_BOUNDSMAXINPIXELST,
	// E_GP_VISMININPIXELST,
	// E_GP_VISMAXINPIXELST,
	//     E_GP_CORNERDISINPIXELS,
	//     E_GP_POWERVALS,
	//     E_GP_POWERVALS2,
	//     E_GP_THICKVALS,
	//     E_GP_MATPARAMS,
	//     E_GP_LENGTH
	// }

	// geomParams[E_GP_BOUNDSMININPIXELST]
	// geomParams[E_GP_BOUNDSMAXINPIXELST]
	// geomParams[E_GP_VISMININPIXELST]
	// geomParams[E_GP_VISMAXINPIXELST]
	// geomParams[E_GP_CORNERDISINPIXELS]
	// geomParams[E_GP_POWERVALS]
	// geomParams[E_GP_POWERVALS2]
	// geomParams[E_GP_THICKVALS]
	// geomParams[E_GP_MATPARAMS]

	// FIVector4 boundsMinInPixelsT;
	// FIVector4 boundsMaxInPixelsT;
	// FIVector4 visMinInPixelsT;
	// FIVector4 visMaxInPixelsT;
	// FIVector4 cornerDisInPixels;
	// FIVector4 powerVals;
	// FIVector4 powerVals2;
	// FIVector4 thickVals;
	// FIVector4 matParams;

	// internal use

	FIVector4 anchorPointInPixels;
	FIVector4 moveMinInPixels;
	FIVector4 moveMaxInPixels;

	GameLight *light;

	bool visible;
	bool hasAnchor;

	//   1
	// 2   0
	//   3


	int rotDir;

	int minRot;
	int maxRot;
	int curRot;
	int buildingType;

	int id;
	int globalId;

	//float minRad;
	//float maxRad;
	//float matId;

	GameGeom() {
		light = NULL;
	}

	void init(int _id) {
		id = _id;
	}

	float getRand() {
		return (fGenRand() + 1.0f) / 2.0f;
	}


	FIVector4 tempVec1;
	FIVector4 tempVec2;


	FIVector4 *getBoundsMinInPixels() {
		return &boundsMinInPixels;
	}
	FIVector4 *getBoundsMaxInPixels() {
		return &boundsMaxInPixels;
	}
	FIVector4 *getVisMinInPixels() {
		return &visMinInPixels;
	}
	FIVector4 *getVisMaxInPixels() {
		return &visMaxInPixels;
	}

	FIVector4 *getBoundsMinInPixelsT() {
		return &geomParams[E_GP_BOUNDSMININPIXELST];
	}
	FIVector4 *getBoundsMaxInPixelsT() {
		return &geomParams[E_GP_BOUNDSMAXINPIXELST];
	}
	FIVector4 *getVisMinInPixelsT() {
		// TODO: make this more efficient and use pixelsPerMeter

		// tempVec1.copyFrom(&geomParams[E_GP_VISMININPIXELST]);
		// tempVec1.addXYZ(pixelsPerMeter);
		// return &tempVec1;

		return &geomParams[E_GP_VISMININPIXELST];
	}
	FIVector4 *getVisMaxInPixelsT() {
		return &geomParams[E_GP_VISMAXINPIXELST];
	}

	int getClampedRot() {
		int tempRot = curRot;
		while (tempRot < 0) {
			tempRot += 4;
		}
		while (tempRot > 3) {
			tempRot -= 4;
		}

		return tempRot;
	}

	void rotate(int mod, bool ignoreConstraints) {

		if (hasAnchor) {
			curRot += mod;


			if (ignoreConstraints) {
				if (curRot > 3) {
					curRot = 0;
				}
				if (curRot < 0) {
					curRot = 3;
				}
			}
			else {
				if (curRot > maxRot) {
					curRot = maxRot - 1;
					rotDir *= -1;
				}
				if (curRot < minRot) {
					curRot = minRot + 1;
					rotDir *= -1;

				}

			}


		}
		else {
			cout << "Attemped to rotate without anchor.\n";
		}


	}

	void initBounds(
		int _buildingType,
		int _id,
		int _globalId,
		int alignBottomMiddleTop,

		float _zOffset,

		FIVector4 *p1,
		FIVector4 *p2,
		FIVector4 *rad,
		FIVector4 *_cornerDisInPixels,
		FIVector4 *_visInsetFromMin,
		FIVector4 *_visInsetFromMax,
		FIVector4 *_powerVals,
		FIVector4 *_powerVals2,
		FIVector4 *_thickVals,
		FIVector4 *_matParams,
		FIVector4 *_centerPoint,
		FIVector4 *_anchorPoint,
		int _minRot,
		int _maxRot

	) {
		buildingType = _buildingType;
		id = _id;
		globalId = _globalId;
		float temp;
		float zOffset = _zOffset;

		curRot = 0;
		rotDir = 1;
		visible = true;
		hasAnchor = false;

		anchorPointInPixels.setFXYZ(0.0f, 0.0f, 0.0f);


		boundsMinInPixels.setFXYZRef(p1);
		boundsMaxInPixels.setFXYZRef(p2);

		FIVector4::normalizeBounds(&boundsMinInPixels, &boundsMaxInPixels);




		boundsMinInPixels.addXYZRef(rad, -1.0f);
		boundsMaxInPixels.addXYZRef(rad, 1.0f);

		switch (alignBottomMiddleTop) {

		case E_ALIGN_BOTTOM: // bottom _@_
			zOffset += (rad->getFZ() - _visInsetFromMin->getFZ());
			break;
		case E_ALIGN_MIDDLE: // middle -@-
			zOffset += 0.0f;
			break;
			//               ___
		case E_ALIGN_TOP: // top  @
			zOffset += -(rad->getFZ() - _visInsetFromMax->getFZ());
			break;


		}

		boundsMinInPixels.addXYZ(0.0f, 0.0f, zOffset);
		boundsMaxInPixels.addXYZ(0.0f, 0.0f, zOffset);


		visMinInPixels.setFXYZRef(&boundsMinInPixels);
		visMaxInPixels.setFXYZRef(&boundsMaxInPixels);

		visMinInPixels.addXYZRef(_visInsetFromMin, 1.0f);
		visMaxInPixels.addXYZRef(_visInsetFromMax, -1.0f);

		geomParams[E_GP_CORNERDISINPIXELS].setFXYZRef(_cornerDisInPixels);
		geomParams[E_GP_POWERVALS].setFXYZRef(_powerVals);
		geomParams[E_GP_POWERVALS2].setFXYZRef(_powerVals2);
		geomParams[E_GP_THICKVALS].setFXYZRef(_thickVals);
		geomParams[E_GP_MATPARAMS].setFXYZRef(_matParams);
		geomParams[E_GP_CENTERPOINT].setFXYZRef(_centerPoint);

		moveMinInPixels.setFXYZRef(&boundsMinInPixels);
		moveMaxInPixels.setFXYZRef(&boundsMaxInPixels);

		geomParams[E_GP_BOUNDSMININPIXELST].setFXYZRef(&boundsMinInPixels);
		geomParams[E_GP_BOUNDSMAXINPIXELST].setFXYZRef(&boundsMaxInPixels);
		geomParams[E_GP_VISMININPIXELST].setFXYZRef(&visMinInPixels);
		geomParams[E_GP_VISMAXINPIXELST].setFXYZRef(&visMaxInPixels);


		if (_minRot != _maxRot) {
			initAnchorPoint(_anchorPoint, _minRot, _maxRot);
		}

	}


	void initTree(
		int _buildingType,
		int _id,
		int _globalId,
		int alignBottomMiddleTop,

		float _zOffset,



		// p0, p1 = start, end
		// p2 = control point or tangent

		FIVector4 *p0,
		FIVector4 *p1,
		FIVector4 *p2,

		float radP0,
		float radP1,
		float sphereRad,

		// FIVector4* rad,
		// FIVector4* _cornerDisInPixels,
		//FIVector4* _visInsetFromMin,
		//FIVector4* _visInsetFromMax,
		// FIVector4* _powerVals,
		// FIVector4* _powerVals2,
		// FIVector4* _thickVals,
		FIVector4 *_matParams



	) {
		buildingType = _buildingType;
		id = _id;
		globalId = _globalId;
		float temp;
		float zOffset = _zOffset;

		float radMax = max(max(radP0, radP1), sphereRad);

		curRot = 0;
		rotDir = 1;
		visible = true;
		hasAnchor = false;

		anchorPointInPixels.setFXYZ(0.0f, 0.0f, 0.0f);


		boundsMinInPixels.setFXYZRef(p0);
		boundsMaxInPixels.setFXYZRef(p1);

		//boundsMinInPixels.addXYZ(-radMax);
		//boundsMaxInPixels.addXYZ(radMax);

		FIVector4::normalizeBounds(&boundsMinInPixels, &boundsMaxInPixels);

		FIVector4::growBoundary(&boundsMinInPixels, &boundsMaxInPixels, p2, p2);

		boundsMinInPixels.addXYZ(-radMax);
		boundsMaxInPixels.addXYZ(radMax);


		// switch (alignBottomMiddleTop) {

		//     case 0: // bottom _@_
		//         zOffset += (radMax-_visInsetFromMin->getFZ());
		//     break;
		//     case 1: // middle -@-
		//         zOffset += 0.0f;
		//     break;
		//             //     ___
		//     case 2: // top  @
		//         zOffset += -(radMax-_visInsetFromMax->getFZ());
		//     break;

		// }

		// boundsMinInPixels.addXYZ(0.0f,0.0f,zOffset);
		// boundsMaxInPixels.addXYZ(0.0f,0.0f,zOffset);


		visMinInPixels.setFXYZRef(&boundsMinInPixels);
		visMaxInPixels.setFXYZRef(&boundsMaxInPixels);

		//visMinInPixels.addXYZRef(_visInsetFromMin, 1.0f);
		//visMaxInPixels.addXYZRef(_visInsetFromMax, -1.0f);

		// geomParams[E_GP_CORNERDISINPIXELS].setFXYZRef(_cornerDisInPixels);
		// geomParams[E_GP_POWERVALS].setFXYZRef(_powerVals);
		// geomParams[E_GP_POWERVALS2].setFXYZRef(_powerVals2);


		geomParams[E_TP_P0].setFXYZRef(p0);
		geomParams[E_TP_P1].setFXYZRef(p1);
		geomParams[E_TP_P2].setFXYZRef(p2);
		geomParams[E_TP_THICKVALS].setFXYZ(radP0, radP1, sphereRad);


		geomParams[E_TP_MATPARAMS].setFXYZRef(_matParams);


		moveMinInPixels.setFXYZRef(&boundsMinInPixels);
		moveMaxInPixels.setFXYZRef(&boundsMaxInPixels);

		// geomParams[E_GP_BOUNDSMININPIXELST].setFXYZRef(&boundsMinInPixels);
		// geomParams[E_GP_BOUNDSMAXINPIXELST].setFXYZRef(&boundsMaxInPixels);
		geomParams[E_TP_VISMININPIXELST].setFXYZRef(&visMinInPixels);
		geomParams[E_TP_VISMAXINPIXELST].setFXYZRef(&visMaxInPixels);


	}



	void applyTransform(int rotMod, bool ignoreConstraints) {

		rotate(rotMod, ignoreConstraints);

		geomParams[E_GP_BOUNDSMININPIXELST].setFXYZRef(&boundsMinInPixels);
		geomParams[E_GP_BOUNDSMAXINPIXELST].setFXYZRef(&boundsMaxInPixels);
		geomParams[E_GP_VISMININPIXELST].setFXYZRef(&visMinInPixels);
		geomParams[E_GP_VISMAXINPIXELST].setFXYZRef(&visMaxInPixels);

		geomParams[E_GP_BOUNDSMININPIXELST].addXYZRef(&anchorPointInPixels, -1.0f);
		geomParams[E_GP_BOUNDSMAXINPIXELST].addXYZRef(&anchorPointInPixels, -1.0f);
		geomParams[E_GP_VISMININPIXELST].addXYZRef(&anchorPointInPixels, -1.0f);
		geomParams[E_GP_VISMAXINPIXELST].addXYZRef(&anchorPointInPixels, -1.0f);

		geomParams[E_GP_BOUNDSMININPIXELST].rotate90(getClampedRot());
		geomParams[E_GP_BOUNDSMAXINPIXELST].rotate90(getClampedRot());
		geomParams[E_GP_VISMININPIXELST].rotate90(getClampedRot());
		geomParams[E_GP_VISMAXINPIXELST].rotate90(getClampedRot());

		geomParams[E_GP_BOUNDSMININPIXELST].addXYZRef(&anchorPointInPixels, 1.0f);
		geomParams[E_GP_BOUNDSMAXINPIXELST].addXYZRef(&anchorPointInPixels, 1.0f);
		geomParams[E_GP_VISMININPIXELST].addXYZRef(&anchorPointInPixels, 1.0f);
		geomParams[E_GP_VISMAXINPIXELST].addXYZRef(&anchorPointInPixels, 1.0f);

		FIVector4::normalizeBounds(&geomParams[E_GP_BOUNDSMININPIXELST], &geomParams[E_GP_BOUNDSMAXINPIXELST]);
		FIVector4::normalizeBounds(&geomParams[E_GP_VISMININPIXELST], &geomParams[E_GP_VISMAXINPIXELST]);

		FIVector4::growBoundary(&moveMinInPixels, &moveMaxInPixels, &geomParams[E_GP_VISMININPIXELST], &geomParams[E_GP_VISMAXINPIXELST]);
	}

	void initAnchorPoint(FIVector4 *_anchorPointInPixels, int _minRot, int _maxRot) {

		int i;


		hasAnchor = true;
		anchorPointInPixels.setFXYZRef(_anchorPointInPixels);
		minRot = _minRot;
		maxRot = _maxRot;

		for (i = 0; i < 4; i++) {
			applyTransform(1, true);
		}

	}



};
