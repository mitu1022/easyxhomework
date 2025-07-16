#include <graphics.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#pragma comment(lib, "MSIMG32.LIB")

#define WIN_LENGTH 1200
#define WIN_WIDTH 800
#define FRAME 60

IMAGE imagebefore, imagebackground, imageplayer, imagebullet, imageenemy, imagehp;
bool mouse_down = false;//�������Ƿ���
int bulletcount = 0;//�ӵ�������
int enemycount = 0;//���˼�����
int scroe = 0;//��ҷ���
bool result = false;
int i = 0, j = 0;

//����ͼƬ
void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h, GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA });
}
//�����
class Player
{
private:
	int x;
	int y;
	int length = 50;
	int width = 50;
	int hp = 3;
	int speed = 10;
	bool ismove_up = false;
	bool ismove_down = false;
	bool ismove_left = false;
	bool ismove_right = false;

public:
	Player() { x = WIN_LENGTH / 2 - length, y = WIN_WIDTH / 2 - width; }

	//��Ϣ����
	void PlayerMessage(ExMessage& msg)
	{
		//���°���
		if (msg.message == WM_KEYDOWN)
		{
			switch (msg.vkcode)
			{
			case 'w':case 'W':case VK_UP:
				ismove_up = true;
				break;

			case 's':case 'S':case VK_DOWN:
				ismove_down = true;
				break;

			case 'a':case 'A':case VK_LEFT:
				ismove_left = true;
				break;

			case 'd':case 'D':case VK_RIGHT:
				ismove_right = true;
				break;
			}
		}
		//�ɿ�����
		if (msg.message == WM_KEYUP)
		{
			switch (msg.vkcode)
			{
			case 'w':case 'W':case VK_UP:
				ismove_up = false;
				break;

			case 's':case 'S':case VK_DOWN:
				ismove_down = false;
				break;

			case 'a':case 'A':case VK_LEFT:
				ismove_left = false;
				break;

			case 'd':case 'D':case VK_RIGHT:
				ismove_right = false;
				break;
			}
		}
	}
	//����ƶ�
	void PlayerMove()
	{
		//�ƶ�
		int direct_x = ismove_right - ismove_left;
		int direct_y = ismove_down - ismove_up;
		double direct_xy = sqrt(direct_x * direct_x + direct_y * direct_y);
		if (direct_xy != 0)
		{
			double factor_x = direct_x / direct_xy;
			double factor_y = direct_y / direct_xy;
			x += (int)speed * factor_x;
			y += (int)speed * factor_y;
		}
		//�����ж�
		if (x < 0) { x = 0; }
		if (x > WIN_LENGTH - length) { x = WIN_LENGTH - length; }
		if (y < 0) { y = 0; }
		if (y > WIN_WIDTH - width) { y = WIN_WIDTH - width; }
	}
	//�������
	void PlayerDraw()
	{
		putimage_alpha(x, y, &imageplayer);
	}
	//�����������
	int GetPlayerX()
	{
		return x;
	}
	int GetPlayerY()
	{
		return y;
	}
	//�������Ѫ��
	int GetPlayerHP()
	{
		return hp;
	}
	//�������Ѫ��
	void PlayerHPReduce()
	{
		hp--;
	}
};
//�ӵ���
class Bullet
{
private:
	int x = 600;
	int y = 400;
	int directX;
	int directY;
	double directXY;
	int length = 20;
	int width = 20;
	int speed = 20;
	bool alive = false;
	bool bevisited = false;
	
public:
	//��Ϣ����
	void BulletMessage(ExMessage& msg)
	{
		if (msg.message == WM_MOUSEMOVE)
		{
			directX = msg.x - x;
			directY = msg.y - y;
		}
		else if(msg.message == WM_LBUTTONDOWN)
		{
			mouse_down = true;
		}
		else if (msg.message == WM_LBUTTONUP)
		{
			mouse_down = false;
		}
		if (mouse_down && bulletcount >= 10)
		{
			alive = true;
			bevisited = true;
			bulletcount = 0;
		}
	}
	//�ӵ��ƶ�
	void BulletMove()
	{
		directXY = sqrt(directX * directX + directY * directY);
		if (directXY != 0)
		{
			double factorX = directX / directXY;
			double factorY = directY / directXY;
			x += (int)speed * factorX;
			y += (int)speed * factorY;
		}
		//�����ж�
		if (x<0 - length || x>WIN_LENGTH || y<0 - width || y>WIN_WIDTH)
		{
			alive = false;
		}
	}
	//�����ӵ�
	void BulletDraw()
	{
		putimage_alpha(x, y, &imagebullet);
	}
	//�ı��ӵ�����״̬
	void ChangeBulletAlive()
	{
		alive = false;
	}
	//����ӵ��Ƿ����
	bool GetBulletAlive()
	{
		return alive;
	}
	//����ӵ��Ƿ񱻷���
	bool GetBulletBevisited()
	{
		return bevisited;
	}
	//�����ӵ�״̬
	void UpdateBullet(Player player)
	{
		if (!alive)
		{
			x = player.GetPlayerX() + 25 - length / 2;
			y = player.GetPlayerY() + 25 - width / 2;
			bevisited = false;
		}
	}
	//�����ӵ�����
	int GetBulletX()
	{
		return x;
	}
	int GetBulletY()
	{
		return y;
	}
};
//������
class Enemy 
{
private:
	int x;
	int y;
	int directX;
	int directY;
	double directXY;
	int length = 50;
	int width = 50;
	int hp = 2;
	int speed = 5;
	bool alive = false;

public:
	//��������
	void CreateEnemy()
	{
		if (enemycount > 45)
		{
			hp = 2;
			alive = true;
			int temp = rand();
			if (temp % 4 == 0)
			{
				x = 0 - length;
				y = temp % WIN_WIDTH;
			}
			if (temp % 4 == 1)
			{
				x = temp % WIN_LENGTH;
				y = WIN_WIDTH;
			}
			if (temp % 4 == 2)
			{
				x = WIN_LENGTH;
				y = temp % WIN_WIDTH;
			}
			if (temp % 4 == 3)
			{
				x = temp % WIN_LENGTH;
				y = 0 - width;
			}
			enemycount = 0;
		}
	}
	//�����ƶ�
	void EnemyMove(Player player)
	{
		directX = player.GetPlayerX() - x;
		directY = player.GetPlayerY() - y;
		directXY = sqrt(directX * directX + directY * directY);
		if (directXY != 0)
		{
			double factorX = directX / directXY;
			double factorY = directY / directXY;
			x += (int)speed * factorX;
			y += (int)speed * factorY;
		}
	}
	//���Ƶ���
	void EnemyDraw()
	{
		putimage_alpha(x, y, &imageenemy);
	}
	//�����˴��״̬
	bool GetEnemyAlive()
	{
		return alive;
	}
	//�������������ײ
	bool Enemy_PlayerCrash(Player player)
	{
		if (x > player.GetPlayerX() - length && x<player.GetPlayerX() + 50 && y>player.GetPlayerY() - width && y < player.GetPlayerY() + 50)
		{
			alive = false;
			return true;
		}
		return false;
	}
	//���������ӵ���ײ
	bool Enemy_BulletCrash(Bullet bullet)
	{
		if (x >= bullet.GetBulletX() - length && x <= bullet.GetBulletX() + 20 && y >= bullet.GetBulletY() - width && y <= bullet.GetBulletY() + 20)
		{
			hp--;
			return true;
		}
		return false;
	}
	//������Ѫ��
	void JudgeEnemyHP()
	{
		if (hp <= 0)
		{
			alive = false;
			scroe++;
		}
	}
};
//����Ѫ��
void PrintHP(Player player)
{
	if (player.GetPlayerHP() >= 3)
	{
		putimage_alpha(100, 0, &imagehp);
	}
	if (player.GetPlayerHP() >= 2)
	{
		putimage_alpha(50, 0, &imagehp);
	}
	if (player.GetPlayerHP() >= 1)
	{
		putimage_alpha(0, 0, &imagehp);
	}
}

int main()
{
	ExMessage msg;//������Ϣ
	Player player;//�������
	Bullet bullet[10];//�����ӵ�
	Enemy enemy[30];//�������

	//����ͼƬ
	loadimage(&imagebefore, _T("image\\imagebefore.png"), WIN_LENGTH, WIN_WIDTH);
	loadimage(&imagebackground, _T("image\\imagebackground.png"), WIN_LENGTH, WIN_WIDTH);
	loadimage(&imageplayer, _T("image\\imageplayer.png"), 50, 50);
	loadimage(&imagebullet, _T("image\\imagebullet.png"), 20, 20);
	loadimage(&imageenemy, _T("image\\imageenemy.png"), 50, 50);
	loadimage(&imagehp, _T("image\\imagehp.png"), 50, 50);

	initgraph(WIN_LENGTH, WIN_WIDTH); //��ʾ����
	HWND hWnd = GetHWnd(); // ��ô��ھ��
	SetWindowText(hWnd, _T("������")); // ʹ�� Windows API �޸Ĵ�������
	
	//��ҳ��
	putimage_alpha(0, 0, &imagebefore);
	while (true)
	{
		while (peekmessage(&msg))
		{
			if (msg.message == WM_KEYDOWN)
			{
				switch (msg.vkcode)
				{
				case'e':case'E':
					i = 1;
				}
			}
		}
		if (i == 1)
		{
			break;
		}
	}
	//��ʼ��ͼ
	BeginBatchDraw();
	while (player.GetPlayerHP() > 0)
	{
		DWORD beginTime = GetTickCount();

		srand(time(NULL));//���������

		//�����ӵ�״̬
		for (i = 0;i < 10;i++)
		{
			bullet[i].UpdateBullet(player);
		}
		//�������
		for (i = 0;i < 30;i++)
		{
			if (!enemy[i].GetEnemyAlive())
			{
				enemy[i].CreateEnemy();
				break;
			}
		}
		//��Ϣ����
		while (peekmessage(&msg))
		{
			player.PlayerMessage(msg);
			for (i = 0;i < 20;i++)
			{
				if (!bullet[i].GetBulletBevisited())
				{
					bullet[i].BulletMessage(msg);
				}
			}
		}
		//�ƶ�
		player.PlayerMove();
		for (i = 0;i < 10;i++)
		{
			if (bullet[i].GetBulletAlive())
			{
				bullet[i].BulletMove();
			}
		}
		for (i = 0;i < 30;i++)
		{
			if (enemy[i].GetEnemyAlive())
			{
				enemy[i].EnemyMove(player);
			}
		}
		//��ײ���
		for (i = 0;i < 30;i++)
		{
			if (enemy[i].GetEnemyAlive())
			{
				if (enemy[i].Enemy_PlayerCrash(player))
				{
					player.PlayerHPReduce();
				}
			}
		}
		for (i = 0;i < 30;i++)
		{
			for (j = 0;j < 10;j++)
			{
				if (enemy[i].GetEnemyAlive() && bullet[j].GetBulletAlive() && enemy[i].Enemy_BulletCrash(bullet[j]))
				{
					bullet[j].ChangeBulletAlive();
				}
			}
		}
		//�жϵ���Ѫ��
		for (i = 0;i < 30;i++)
		{
			if (enemy[i].GetEnemyAlive())
			{
				enemy[i].JudgeEnemyHP();
			}
		}

		bulletcount++;//�����ӵ�������
		enemycount++;//���µ��˼�����

		//��ͼ
		cleardevice();
		putimage_alpha(0, 0, &imagebackground);
		player.PlayerDraw();
		for (i = 0;i < 10;i++)
		{
			if (bullet[i].GetBulletAlive())
			{
				bullet[i].BulletDraw();
			}
		}
		for (i = 0;i < 30;i++)
		{
			if (enemy[i].GetEnemyAlive())
			{
				enemy[i].EnemyDraw();
			}
		}
		PrintHP(player);

		FlushBatchDraw();
		//��Ϸʤ���ж�
		if (scroe >= 20)
		{
			result = true;
			break;
		}
		//֡��ʱ
		DWORD endTime = GetTickCount();
		DWORD elapsedTime = endTime - beginTime;
		if (elapsedTime < 1000 / FRAME)
		{
			Sleep(1000 / FRAME - elapsedTime);
		}
	}
	EndBatchDraw();
	//��������
	if (result == true)
	{
		MessageBox(hWnd, _T("��ʤ���ˣ�"), _T("��Ϸ����"), MB_OK | MB_ICONERROR);
	}
	else
	{
		MessageBox(hWnd, _T("�㱻�������ˣ�"), _T("��Ϸ����"), MB_OK | MB_ICONERROR);
	}
	closegraph();
	return 0;
}