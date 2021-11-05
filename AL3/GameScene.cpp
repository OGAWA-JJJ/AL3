#include "GameScene.h"
#include "stdio.h"
#include "time.h"

int GameScene::No;

GameScene::GameScene()
{
	No = Title;

	model3D.CreateDescriptorHeap();
	model3D.CreateGraphicsPipeline();
	model3D.Init();
	model3D.CreateModel("RafaleB");

	for (int i = 0; i < MaxBulletNum + MaxCube; i++) {
		//20~24 obj
		if (i >= MaxBulletNum) {
			NormalBullet[i].CreateCube(10.0f, L"Resources/Box.png");
		}

		//0~19 Bullet
		else
			NormalBullet[i].CreateCube(0.5f, L"Resources/Bullet.png");
	}

	NormalBullet[25].CreateCube(1050, L"Resources/Space.png");

	for (int i = 0; i < _countof(d); i++) {
		d[i].PipelineStateObjInit();
		d[i].sprData.texNumber = i;

		if (i == 0)
			d[i].SpriteLoadTexture(i, L"Resources/Radar.png");
		else if (i == 1)
			d[i].SpriteLoadTexture(i, L"Resources/enemy2.png");
		else if (i == 2)
			d[i].SpriteLoadTexture(i, L"Resources/player2.png");
		else if (i == 3)
			d[i].SpriteLoadTexture(i, L"Resources/Crosshair.png");
		else if (i == 4)
			d[i].SpriteLoadTexture(i, L"Resources/Destroy.png");
		else if (i == 5)
			d[i].SpriteLoadTexture(i, L"Resources/Warning.png");
		else if (i == 6)
			d[i].SpriteLoadTexture(i, L"Resources/PullUp.png");
		else if (i == 7)
			d[i].SpriteLoadTexture(i, L"Resources/Start.png");
		else if (i == 8)
			d[i].SpriteLoadTexture(i, L"Resources/Title.png");

		else if (i == 9)
			d[i].SpriteLoadTexture(i, L"Resources/enemy2.png");
		else if (i == 10)
			d[i].SpriteLoadTexture(i, L"Resources/enemy2.png");
		else if (i == 11)
			d[i].SpriteLoadTexture(i, L"Resources/enemy2.png");
		else if (i == 12)
			d[i].SpriteLoadTexture(i, L"Resources/enemy2.png");
		else if (i == 13)
			d[i].SpriteLoadTexture(i, L"Resources/Clear.png");


		d[i].CreateSpriteInit();
	}

	d[0].sprData.spritePosition = { 128,592,0 };
	d[1].sprData.spritePosition = { 32,32,0 };
	d[2].sprData.spritePosition = { 64,646,0 };
	d[3].sprData.spritePosition = { 640,360,0 };
	d[4].sprData.spritePosition = { 640,360,0 };
	d[5].sprData.spritePosition = { 640,224,0 };
	d[6].sprData.spritePosition = { 640,288,0 };
	d[7].sprData.spritePosition = { 640,360,0 };
	d[8].sprData.spritePosition = { 640,360,0 };

	d[13].sprData.spritePosition = { 640,360,0 };

	//Enemy
	srand((unsigned)time(NULL));
	random[0] = rand() % 2000 - 1000;
	random[1] = rand() % 2000 - 1000;
	random[2] = rand() % 2000 - 1000;
	NormalBullet[20].position = { random[0],random[1],random[2] };
	random[0] = rand() % 2000 - 1000;
	random[1] = rand() % 2000 - 1000;
	random[2] = rand() % 2000 - 1000;
	NormalBullet[21].position = { random[0],random[1],random[2] };
	random[0] = rand() % 2000 - 1000;
	random[1] = rand() % 2000 - 1000;
	random[2] = rand() % 2000 - 1000;
	NormalBullet[22].position = { random[0],random[1],random[2] };
	random[0] = rand() % 2000 - 1000;
	random[1] = rand() % 2000 - 1000;
	random[2] = rand() % 2000 - 1000;
	NormalBullet[23].position = { random[0],random[1],random[2] };
	random[0] = rand() % 2000 - 1000;
	random[1] = rand() % 2000 - 1000;
	random[2] = rand() % 2000 - 1000;
	NormalBullet[24].position = { random[0],random[1],random[2] };

	for (int i = 0; i < MaxCube; i++)
		EnemyHP[i] = 5;
}

void GameScene::SceneChaneTitle()
{
	No = Title;
}

void GameScene::SceneChaneGamePlay()
{
	No = GamePlay;
}

void GameScene::SceneChangeClear()
{
	No = Clear;
}

void GameScene::Bullet(XMFLOAT3 MyPosition, XMMATRIX matRot)
{
	XMFLOAT3 V0 = { 0,0,1 };
	//bool MissileFlag[MaxMissileNum] = {};

	if (Input::isKey(DIK_SPACE)) {
		//”­ŽË€”õ
		for (int i = 0; i < MaxBulletNum; i++) {
			if (!BulletFlag[i]) {
				if (BulletCount <= 0) {
					NormalBullet[i].position = MyPosition;

					V[i] = { V0.x,V0.y,V0.z,0 };

					//•ûŒüƒxƒNƒgƒ‹~‰ñ“]s—ñ
					V[i] = XMVector3Transform(V[i], matRot);

					//³‹K‰»
					V[i] = XMVector3Normalize(V[i]);

					BulletFlag[i] = true;

					BulletCount = 0.5f;
					break;
				}
			}
		}
	}

	//”­ŽË
	for (int i = 0; i < MaxBulletNum; i++) {
		if (BulletFlag[i]) {
			F[i] = { V[i].m128_f32[0],V[i].m128_f32[1], V[i].m128_f32[2] };
			NormalBullet[i].position.x += F[i].x * Speed;
			NormalBullet[i].position.y += F[i].y * Speed;
			NormalBullet[i].position.z += F[i].z * Speed;

			for (int j = MaxBulletNum; j < MaxBulletNum + MaxCube; j++) {
				if (BulletHitCheak(NormalBullet[i].position, NormalBullet[j].position)) {
					if (EnemyHP[j - MaxBulletNum] != 0)
						EnemyHP[j - MaxBulletNum] -= 1;
					music.PlayWave("Resources/se_mod05.wav", 0.1f);
					if (EnemyHP[j - MaxBulletNum] == 0)
						DestroyFlag = true;
					BulletFlag[i] = false;
				}

				if (NormalBullet[i].position.x > 1000
					|| NormalBullet[i].position.x < -1000
					|| NormalBullet[i].position.y>1000
					|| NormalBullet[i].position.y < -1000
					|| NormalBullet[i].position.z>1000
					|| NormalBullet[i].position.z < -1000) {
					BulletFlag[i] = false;
				}
			}
		}
	}

	BulletCount -= 0.1f;
}

bool GameScene::BulletHitCheak(XMFLOAT3 BulletPosition, XMFLOAT3 EnemyPosition)
{
	const float radius1 = 1.0f;
	const float radius2 = 10.0f;

	XMFLOAT3 position1;
	XMFLOAT3 position2;
	float distance;
	XMVECTOR position_sub;

	//’e
	position1 = BulletPosition;

	//“G
	position2 = EnemyPosition;

	position_sub = XMVectorSet(
		position1.x - position2.x,
		position1.y - position2.y,
		position1.z - position2.z,
		0);

	position_sub = XMVector3Length(position_sub);
	distance = position_sub.m128_f32[0];

	//“–‚½‚Á‚Ä‚¢‚½‚ç
	if (distance <= radius1 + radius2) {
		return true;
	}
	else {
		return false;
	}
}

void GameScene::Update()
{
	for (int i = 0; i < _countof(d); i++) {
		d[i].SpriteUpdate();
	}

	if (No == Title) {
		d[8].SpriteDraw(true);
		if (Input::isKeyTrigger(DIK_RETURN))
			No = GamePlay;
	}

	else if (No == GamePlay) {
		bool flag = true;
		float speed = 3.0f;
		if (!MusicFlag) {
			MusicFlag = true;
			music.PlayWave("Resources/loop100203.wav", 0.3f);
		}


		if (Input::isKey(DIK_D) || Input::isKey(DIK_A) || Input::isKey(DIK_W) || Input::isKey(DIK_S)) {
			if (Input::isKey(DIK_D)) {
				model3D.rotation.y += 2.0f;
			}
			if (Input::isKey(DIK_A)) {
				model3D.rotation.y -= 2.0f;
			}
			if (Input::isKey(DIK_W)) {
				model3D.rotation.x += 2.0f;
			}
			if (Input::isKey(DIK_S)) {
				model3D.rotation.x -= 2.0f;
			}
		}

		//ƒfƒtƒHƒ‹ƒg‚Ì•ûŒüƒxƒNƒgƒ‹‚ðŒvŽZ—pXMVECTOR‚É•ÏŠ·
		XMFLOAT3 V0 = { 0,0,1 };
		XMVECTOR V = { V0.x,V0.y,V0.z,0 };

		//•ûŒüƒxƒNƒgƒ‹~‰ñ“]s—ñ
		V = XMVector3Transform(V, model3D.matRot);

		//³‹K‰»
		V = XMVector3Normalize(V);

		//XMFLOAT3‚É–ß‚µ‚ÄŒvŽZ
		XMFLOAT3 F = { V.m128_f32[0],V.m128_f32[1], V.m128_f32[2] };
		model3D.position.x += F.x * speed;
		model3D.position.y += F.y * speed;
		model3D.position.z += F.z * speed;

		Camera::target = model3D.position;

		model3D.Update();

		Bullet(model3D.position, model3D.matRot);

		d[2].sprData.spriteRotation = model3D.rotation.y;

		if (StartFlag || DestroyFlag || PullUpFlag || WarningFlag) {
			DrawTimer -= 0.1f;
		}
		if (DrawTimer <= 0.0f) {
			StartFlag = false;
			DestroyFlag = false;
			PullUpFlag = false;
			WarningFlag = false;
			DrawTimer = 5.0f;
		}

		if (model3D.position.y < -800)
			PullUpFlag = true;
		else
			PullUpFlag = false;

		if (model3D.position.y > 800
			|| model3D.position.x < -800
			|| model3D.position.x>800
			|| model3D.position.z < -800
			|| model3D.position.z>800)
			WarningFlag = true;
		else
			WarningFlag = false;

		if (model3D.position.x > 1000
			|| model3D.position.x < -1000
			|| model3D.position.y>1000
			|| model3D.position.y < -1000
			|| model3D.position.z>1000
			|| model3D.position.z < -1000) {
			model3D.position = { 0,0,0 };
		}

		float posX = 0;
		float posY = 0;

		posX = (model3D.position.x + 1000) * 0.128;
		posY = (model3D.position.z - 1000) * -0.128 + 464;
		d[2].sprData.spritePosition = { posX,posY,0 };

		for (int i = 0; i < MaxCube; i++) {
			EnemyPosX[i] = (NormalBullet[i + MaxBulletNum].position.x + 1000) * 0.128;
			EnemyPosY[i] = (NormalBullet[i + MaxBulletNum].position.z - 1000) * -0.128 + 464;
		}

		if (EnemyHP[0] == 0 && EnemyHP[1] == 0 && EnemyHP[2] == 0 && EnemyHP[3] == 0 && EnemyHP[4] == 0) {
			No = Clear;
		}
	}

	else if (No == Clear) {
		//‰Šú‰»
		model3D.rotation = { 0,0,0 };
		for (int i = 0; i < MaxCube; i++) {
			EnemyHP[i] = 5;
		}
		//Enemy
		srand((unsigned)time(NULL));
		random[0] = rand() % 2000 - 1000;
		random[1] = rand() % 2000 - 1000;
		random[2] = rand() % 2000 - 1000;
		NormalBullet[20].position = { random[0],random[1],random[2] };
		random[0] = rand() % 2000 - 1000;
		random[1] = rand() % 2000 - 1000;
		random[2] = rand() % 2000 - 1000;
		NormalBullet[21].position = { random[0],random[1],random[2] };
		random[0] = rand() % 2000 - 1000;
		random[1] = rand() % 2000 - 1000;
		random[2] = rand() % 2000 - 1000;
		NormalBullet[22].position = { random[0],random[1],random[2] };
		random[0] = rand() % 2000 - 1000;
		random[1] = rand() % 2000 - 1000;
		random[2] = rand() % 2000 - 1000;
		NormalBullet[23].position = { random[0],random[1],random[2] };
		random[0] = rand() % 2000 - 1000;
		random[1] = rand() % 2000 - 1000;
		random[2] = rand() % 2000 - 1000;
		NormalBullet[24].position = { random[0],random[1],random[2] };

		if (Input::isKeyTrigger(DIK_RETURN))
			No = Title;
	}
}

void GameScene::Draw()
{
	if (No == GamePlay) {
		for (int i = 0; i < _countof(d) - 1; i++) {
			if (i < 4)
				if (i == 1) {
					d[i].sprData.spritePosition = { EnemyPosX[0],EnemyPosY[0],0 };
					d[i + 8].sprData.spritePosition = { EnemyPosX[1],EnemyPosY[1],0 };
					d[i + 9].sprData.spritePosition = { EnemyPosX[2],EnemyPosY[2],0 };
					d[i + 10].sprData.spritePosition = { EnemyPosX[3],EnemyPosY[3],0 };
					d[i + 11].sprData.spritePosition = { EnemyPosX[4],EnemyPosY[4],0 };

					if (EnemyHP[0] != 0)
						d[i].SpriteDraw(true);
					if (EnemyHP[1] != 0)
						d[i + 8].SpriteDraw(true);
					if (EnemyHP[2] != 0)
						d[i + 9].SpriteDraw(true);
					if (EnemyHP[3] != 0)
						d[i + 10].SpriteDraw(true);
					if (EnemyHP[4] != 0)
						d[i + 11].SpriteDraw(true);
				}
				else
					d[i].SpriteDraw(true);

			if (i == 4 && DestroyFlag)
				d[i].SpriteDraw(true);
			else if (i == 5 && WarningFlag)
				d[i].SpriteDraw(true);
			else if (i == 6 && PullUpFlag)
				d[i].SpriteDraw(true);
			else if (i == 7 && StartFlag)
				d[i].SpriteDraw(true);
		}

		model3D.Draw();

		for (int i = 0; i < MaxBulletNum; i++) {
			if (BulletFlag[i])
				NormalBullet[i].DrawCube();
		}

		for (int i = MaxBulletNum; i < MaxBulletNum + MaxCube; i++) {
			if (EnemyHP[i - MaxBulletNum] > 0) {
				NormalBullet[i].DrawCube();
			}
		}

		NormalBullet[25].DrawCube();
	}

	else if (No == Clear) {
		d[13].SpriteDraw();
	}
}
