#include "stdafx.h"
#include "block.h"
#include "message.h"

//This routine manages the window that displays the Icon flow chart.
//It must perform the following functions:
//Keep Track of:
//     which Icons are fixed,
//     which are variable and 
//     which are displayed
//
//Respond to:
//     Messages from the parent changing states
//     Messages from the parent asking for information
//     Mouse messages trying to move Icons about.
//
//Display:
//     Which Icons are fixed
//     Which Icons are variable
//     Icon Order.
//     
//
//Messages: This block responds to the following messages

//CMD_BLOCK_SET: Setup the valid info on the block.
//  wParam= Block ID number (1..6)
//  lParam= MAKELONG(hvicon, hcursor);
//

//CMD_BLOCK_FIXED  Set weither the block is in the fixed or variable area.
//  wParam= Block ID number (1..6)
//  lParam= 1 or 0 ,1=Fixed 0=Variable
// 


//CMD_BLOCK_SET_ORDER  Used when saving or retrieving files to set the block order   
// wParam= NA
// lParam= Order 00123456h would have block #1 as 6th 2 as fifth etc...

//CMD_BLOCK_GET_ORDER  Used by the host to determine the running order.
// wParam= NA
// lParam= Order 00123456h would have block #1 as 6th 2 as fifth etc...

//A note about actions:
//
//The entry can be in three states:
//
//              ³SET_BLOCK=On³SET_BLOCK=OFF³SET_FIXED=FIXED³SET_FIXED=VAR ³
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
//Never Shown   ³Install in  ³Nothing      ³Nothing        ³Nothing       ³
//or Off        ³fixed/var   ³             ³               ³              ³
//              ³list and    ³             ³               ³              ³
//              ³redraw      ³             ³               ³              ³
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
//In fixed list ³redraw      ³remove from  ³Nothing        ³remove from   ³
//              ³icon only   ³list and     ³               ³fixed list add³
//              ³            ³redraw       ³               ³to var list   ³
//              ³            ³             ³               ³and redraw    ³
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´
//In var list   ³redraw      ³remove from  ³remove from var³Nothing       ³
//              ³icon only   ³list and     ³list, add to   ³              ³
//              ³            ³redraw       ³fixed list and ³              ³
//              ³            ³             ³redraw         ³              ³
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
                                                                           
		       

static int drag_error_count;

void Outline_Icon(HDC hDc,HICON hIcon,int x, int y)
{
	#define Offset 2
	DrawIcon(hDc,x,y,hIcon);
	POINT ptOld;
	MoveToEx(hDc,x-Offset,y-Offset, &ptOld);
	LineTo(hDc,x-Offset,y+32);
	LineTo(hDc,x+32,y+32);
	LineTo(hDc,x+32,y-Offset);
	LineTo(hDc,x-Offset,y-Offset);
}

typedef struct FLOWBLOCK_TAG
{
	BOOL    fixed;   //Is the block fixed?
	BOOL    shown;   //Is it shown?
	HICON   hicon;   //Its icon
	HCURSOR hcursor; //Its cursor
	int     level;   //The level in the structure
	int     n;       //The number in the array for use with the pointer list
	int     x;       //The x position of the block
	int     y;       //The y position of the block
	int    ox;       //The last x;
	int    oy;       //The last y;
	int    next_block; //Pointer to the next block
	int    prev_block; //Pointer to the previous block
}FLOWBLOCK;

#define N_BLOCKS 8
static FLOWBLOCK    block_list[N_BLOCKS];    //The list of blocks to operate on
static int level_head;    //The head of the list at level 0
static int fixed_head;    //The head of the fixed list.
static int xpo[3], ypo[N_BLOCKS+1];
static int nfixed;
static int nvar;
static int changed;
static int yBetweenIcons = 4;

int Block_Changed(void)
{
	if (changed) 
	{
		changed=0; 
		return 1; 
	}

	changed=0;
	return 0;
}

void regen_display(HWND hwnd, int setoff=0)
{
	int temp;
	int x;
	int y;
	int l;
	int ofixed=nfixed;
	int ovar=nvar;
	HDC hDc;
	x=0;
	y=0;
	nfixed=0;
	temp=fixed_head;
	while (temp !=-1)
	{
		block_list[temp].level=0;
		block_list[temp].x=x;
		block_list[temp].y=y;
		nfixed++;
		if (x) 
		{
			y++; 
			x=0;
		}
		else 
			x=2;
		temp=block_list[temp].next_block;
	}
	if (x!=0) y++;
	if (nfixed<=1) x=0;
	else x=1;
	l=1;
	temp=level_head;
	while (temp !=-1)
	{
		block_list[temp].level=l++;
		block_list[temp].x=x;
		block_list[temp].y=y;
		y++;
		temp=block_list[temp].next_block;
	}

	if (setoff)
	{
		for (int i=0; i<N_BLOCKS; i++) 
		{
			block_list[i].ox=block_list[i].x;
			block_list[i].oy=block_list[i].y;
		}
	}

	nvar=l;
	if ((nvar==ovar) && (nfixed==ofixed))
	{
		//redo icons only
		hDc=GetDC(hwnd);
		l=level_head;
		while(l!=-1)
		{  
			if ((block_list[l].x!=block_list[l].ox) || (block_list[l].y!=block_list[l].oy))
			{
				Outline_Icon(hDc,block_list[l].hicon,xpo[block_list[l].x],ypo[block_list[l].y]);
				block_list[l].ox=block_list[l].x;
				block_list[l].oy=block_list[l].y;
			}
			l=block_list[l].next_block;
		}
		ReleaseDC(hwnd,hDc);
	}

	RECT rc;
	GetClientRect(hwnd, &rc);
	RedrawWindow(hwnd, &rc, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
}



void remove_from_list(int block)
{
	if (block_list[block].next_block!=-1)
		block_list[block_list[block].next_block].prev_block=block_list[block].prev_block;

	if (block_list[block].prev_block!=-1)
		block_list[block_list[block].prev_block].next_block=block_list[block].next_block;
	else
	{
		if (level_head==block) 
			level_head=block_list[block].next_block;
		else if (fixed_head==block)
			fixed_head=block_list[block].next_block;
	}
}


void add_to_list(int block,int &list)
{
	block_list[block].prev_block=-1;
	block_list[block].next_block=list;
	if (list !=-1) 
		block_list[list].prev_block=block;
	list=block;
}

int find_hit(int x, int y)
{
	int i;
	int f=0;
	for (i=0; ((i<N_BLOCKS) && (!f)); i++)
	{
		if (block_list[i].shown)
		{
			if ((x>=xpo[block_list[i].x]) &&
				(y>=ypo[block_list[i].y]) &&
				(x<=(xpo[block_list[i].x]+32)) &&
				(y<=(ypo[block_list[i].y]+32))) 
				f=i;
		}
	}
	return f;
}

void Insert_Before(int what, int where)
{
	block_list[what].next_block=where;
	block_list[what].prev_block=block_list[where].prev_block;

	if (block_list[where].prev_block!=-1) 
		block_list[block_list[where].prev_block].next_block=what;
	else if (level_head==where) 
		level_head=what;
	else
		MessageBox(NULL, "Internal error ", "Insert Before", MB_OK|MB_SYSTEMMODAL);

	block_list[where].prev_block=what;
}



void Insert_After(int what, int where)
{
	block_list[what].prev_block=where;
	block_list[what].next_block=block_list[where].next_block;
	if (block_list[where].next_block!=-1) 
		block_list[block_list[where].next_block].prev_block=what;
	block_list[where].next_block=what;
}

void Arrow_Dn(HDC hDc,int x, int y)
{
	POINT ptOld;
	MoveToEx(hDc,x+3,y-3,&ptOld);//Down Arrow
	LineTo(hDc,x,y);       //Down Arrow
	LineTo(hDc,x-3,y-3); //Down Arrow
}

void Arrow_Up(HDC hDc,int x, int y)
{
	POINT ptOld;
	MoveToEx(hDc,x+3,y+3,&ptOld); //Up Arrow
	LineTo(hDc,x,y);       //Up Arrow
	LineTo(hDc,x-3,y+3); //Up Arrow
}

long __declspec(dllexport) CALLBACK	BlockWndProc(HWND hwnd,	UINT message, WPARAM wParam, LPARAM lParam)
{
    #define x_ofs 8
    #define y_ofs 8
    int	x, y,y2, i, j,l;
    int	dx, dy;
    static HCURSOR old_cursor;
    static int dragging;
    BOOL old_var;
    RECT rect;
    PAINTSTRUCT	ps;
    HDC hDc;

   switch (message) 
   {
	case WM_CREATE : 
	{
		GetClientRect(hwnd, &rect);
		old_cursor=NULL;
		dragging=0;
		changed=FALSE;

		rect.top += 25;
		rect.left += 4;
		dx = (rect.right - rect.left); //Calculate Width
		dy = (rect.bottom - rect.top); //Calculate Availible Height
		x = dx - 64;

		xpo[0] = rect.left + (x	/ 4);          //Col 0 X position
		xpo[1] = rect.left + ((dx - 32)	/ 2);  //Col 1 X position
		xpo[2] = xpo[0]	+ (x / 2) + 32;        //Col 2 X position

		y = (dy - ((N_BLOCKS-1) * 32)) / (N_BLOCKS-1);

		if (yBetweenIcons < y) 
			yBetweenIcons = y;
		ypo[0] = rect.top + (yBetweenIcons / 2);            //Row 0 Y position
		for (i = 1; i <	N_BLOCKS; i++)
			ypo[i] = ypo[i - 1] + yBetweenIcons + 32;        //Row n Y position

		fixed_head=-1;
		level_head=-1;

		for (i=0; i<N_BLOCKS; i++)
		{ 
			block_list[i].fixed=TRUE;
			block_list[i].shown=FALSE;
			block_list[i].hicon=(HICON)-1;
			block_list[i].hcursor=(HCURSOR)-1;
			block_list[i].level=0;
			block_list[i].n=i;
			block_list[i].x=0;
			block_list[i].y=0;
		}

		return 0;
	}

    case WM_DESTROY: 
    {
       return 0;
    }

    case CMD_BLOCK_SET : 
		if ((wParam > N_BLOCKS) | (wParam < 1))
		{
			MessageBox(hwnd, "Bad block ", "CMD_BLOCK_SET", MB_OK);
			return 0;
		}

		old_var=block_list[wParam].shown;
		block_list[wParam].hicon=(HICON)LOWORD(lParam);
		block_list[wParam].hcursor=(HCURSOR)HIWORD(lParam);
		block_list[wParam].shown=(LOWORD(lParam)!=0);

		if (!(block_list[wParam].shown))
		{
			//SET_BLOCK==OFF
			if (old_var)//Was shown
			{  
				remove_from_list(wParam);
				regen_display(hwnd);
			}
			block_list[wParam].level=0;
			return 0;
		}
		else
		{
			//SET_BLOCK==ON
			if (old_var)//was shown
			{  
				hDc=GetDC(hwnd);
				Outline_Icon(hDc,block_list[wParam].hicon,xpo[block_list[wParam].x],ypo[block_list[wParam].y]);
				ReleaseDC(hwnd,hDc);
			}
			else
			{  
				if (block_list[wParam].fixed) 
					add_to_list(wParam,fixed_head);
				else
					add_to_list(wParam,level_head);
				regen_display(hwnd);
			}
		}
		return 0;

    case CMD_BLOCK_FIXED : 
		if ((wParam > N_BLOCKS) | (wParam < 1))
		{  
			MessageBox(hwnd, "Bad block ", "CMD_BLOCK_FIXED", MB_OK);
			return 0;
		}

		old_var=block_list[wParam].fixed;
		block_list[wParam].fixed=lParam;
		if ((old_var!=lParam)&&(block_list[wParam].shown))
		{  
			remove_from_list(wParam);
			if (lParam) 
				add_to_list(wParam,fixed_head);
			else 
				add_to_list(wParam,level_head);
			regen_display(hwnd);
		}
		return 0;

    case CMD_BLOCK_GET_ORDER:
       lParam = 0;
       for (i = 0; i < N_BLOCKS; i++)
          lParam = (lParam * 16) + (block_list[i].level & 15);
       return lParam;

    case CMD_BLOCK_SET_ORDER:
		{ 
			int la[N_BLOCKS+1];
			fixed_head=-1;
			for (i=0; i<=N_BLOCKS; i++)
				la[i]=0;

			for	(i = N_BLOCKS -	1; i >0; i--)
			{
				block_list[i].level = (lParam &15);
				if (block_list[i].level==0)
				{
					if (block_list[i].shown)
					{  
						add_to_list(i,fixed_head);
						block_list[i].fixed=TRUE;
					}
				}
				else 
				{  
					la[block_list[i].level]=i;
					block_list[i].fixed=FALSE;
				}

				lParam = lParam / 16;
			}

			//Now all the levels and the fixed list is ok.
			level_head=-1;
			i=1;
			while(la[i])
			{  
				if (i==1) 
					add_to_list(la[i],level_head);
				else  
					Insert_After(la[i],la[i-1]);
				i++;
			}

			regen_display(hwnd,1);
			return 0;
		}

     case WM_LBUTTONDOWN:
		{	
			x = LOWORD(lParam);
			y = HIWORD(lParam);
			i=find_hit(x,y);  
				 
			if (i)
			{  
				if (block_list[i].fixed) 
				{  
					drag_error_count++;
					/*
					if (drag_error_count>5)
						MessageBox(NULL, "You Can't Touch that", "And the beat goes on", MB_OK|MB_TASKMODAL);
					else 
						MessageBox(NULL, "You Can't Drag it", "That test has a fixed value ", MB_OK|MB_TASKMODAL);
					*/
				}
				else
				{
					old_cursor = SetCursor(block_list[i].hcursor);
					dragging = i;
					SetCapture(hwnd);
					changed=1;
				}
			}
			else 
				MessageBeep(0);
			return 0;
		}

     case WM_LBUTTONUP:
		if (!old_cursor) return 0;
		ReleaseCapture();
		changed=1;
		SetCursor(old_cursor);
		old_cursor=NULL;
		x =	LOWORD(lParam);
		y =	HIWORD(lParam);
		i=find_hit(x,y);
		if ((i!=0) && (i!=dragging) && (!(block_list[i].fixed)))
		{  
			remove_from_list(dragging);
			if (block_list[i].level>block_list[dragging].level) 
				Insert_After(dragging,i);
			else 
				Insert_Before(dragging,i);
			regen_display(hwnd);
		}
		dragging=0;
        return 0;

     case WM_PAINT:
		hDc	= BeginPaint(hwnd, &ps);
		GetClientRect(hwnd,	&rect);
		SelectObject(hDc, GetStockObject(BLACK_PEN));
		if (nfixed)
		{
			i=fixed_head;
			while (i!=-1)
			{  
				y=block_list[i].y;
				i=block_list[i].next_block;
			}
			HFONT hOldFont = NULL;
			if (g_hMainDlg != NULL)
			{
				HFONT hFont = (HFONT) SendMessage(g_hMainDlg, WM_GETFONT, 0, 0);
				hOldFont = (HFONT) SelectObject(hDc, hFont);
			}
			SetBkMode(hDc, TRANSPARENT); 
 			TextOut(hDc, xpo[1], ypo[0] - 20, "Fixed", 5);
			if (hOldFont != NULL)
				SelectObject(hDc, hOldFont);
		}
		else
		{  
			POINT ptOld;
			MoveToEx(hDc,xpo[0]+16,ypo[0],&ptOld);
			LineTo(hDc,xpo[0]+16,ypo[0]-((ypo[1]-ypo[0]-32)/2));
		}
		
		for (i=0; i<N_BLOCKS; i+=1)
		{
			if (block_list[i].shown)
			{  
				Outline_Icon(hDc,block_list[i].hicon,xpo[block_list[i].x],ypo[block_list[i].y]);
				block_list[i].ox=block_list[i].x;
				block_list[i].oy=block_list[i].y;

				if ((!block_list[i].fixed) && (block_list[i].y))
				{
					POINT ptOld;
					MoveToEx(hDc,xpo[block_list[i].x]+16,ypo[block_list[i].y-1]+33,&ptOld);
					LineTo(hDc,xpo[block_list[i].x]+16,ypo[block_list[i].y]-1);
					y=(ypo[block_list[i].y]+ypo[block_list[i].y-1]+32)/2;
					x=xpo[block_list[i].x]+16;
					if (yBetweenIcons>=8) 
						Arrow_Dn(hDc,x,y);
				}
			}
		}

		dy=ypo[2]-ypo[3];
		if (nvar>1)
		{  
			i=level_head;
			while ((i!=-1) && (block_list[i].next_block!=-1))
				i=block_list[i].next_block;

			//i now = the last block on the variable list.
			j=block_list[i].level;
			//j now = the number of levels to go up
			x=xpo[block_list[i].x]+16;
			y=ypo[block_list[i].y]+32;  

			int yb = y;
			GetClientRect(hwnd, &rect);
			if (y+5 < rect.bottom) yb = y+5;

			POINT ptOld;
			MoveToEx(hDc,x,y,&ptOld);
			LineTo(hDc,x,yb);
			LineTo(hDc,x+16+(8*j),yb);
			l=block_list[i].y;
			y2=(ypo[l]+ypo[l-1]+32)/2; 

			for (i=1; i<=j; i++)
			{  
				MoveToEx(hDc,x+16+(8*i),yb,&ptOld);
				LineTo(hDc,x+16+(8*i),y2+(dy*i)-dy);
				LineTo(hDc,x,y2+(dy*i)-dy);
				Arrow_Up(hDc,x+16+(8*i),(y+y2+(dy*i)-dy)/2);
			}
		}

		EndPaint(hwnd, &ps);
		return 0;
   }

   return (DefWindowProc(hwnd,	message, wParam, lParam));
}


