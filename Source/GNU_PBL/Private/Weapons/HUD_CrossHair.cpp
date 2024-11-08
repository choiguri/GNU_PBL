// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/HUD_CrossHair.h"
#include "Engine/Engine.h"
#include "Engine/Canvas.h"           
#include "Engine/GameViewportClient.h" 
#include "Math/Vector2D.h"   

AHUD_CrossHair::AHUD_CrossHair()
{
	Spread = FVector2D(10.f, 10.f);
}


void AHUD_CrossHair::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize
		(ViewportSize);

		const FVector2D ViewportCenter(ViewportSize.X / 2.f,
			ViewportSize.Y / 2.f);

        // 크로스헤어 사각형 그리기
        const float BoxSize = 5.f;  // 사각형 크기 설정
        const FLinearColor BoxColor = FLinearColor::White;  // 사각형 색상

        // 중앙에 하얀 박스 그리기
        DrawRect(
            BoxColor,
            ViewportCenter.X - (BoxSize / 2.f),
            ViewportCenter.Y - (BoxSize / 2.f),
            BoxSize,
            BoxSize
        );

        // 추가적으로 크로스헤어의 네 방향에 퍼져나가는 선을 그리기
        const float LineThickness = 2.f;  // 선의 두께
        const float LineLength = 10.f + Spread.X;  // 선의 길이 (Spread에 따라 퍼짐)

        // 위쪽 선 그리기
        DrawRect(
            BoxColor,
            ViewportCenter.X - (LineThickness / 2.f),
            ViewportCenter.Y - LineLength,
            LineThickness,
            LineLength
        );

        // 아래쪽 선 그리기
        DrawRect(
            BoxColor,
            ViewportCenter.X - (LineThickness / 2.f),
            ViewportCenter.Y,
            LineThickness,
            LineLength
        );

        // 왼쪽 선 그리기
        DrawRect(
            BoxColor,
            ViewportCenter.X - LineLength,
            ViewportCenter.Y - (LineThickness / 2.f),
            LineLength,
            LineThickness
        );

        // 오른쪽 선 그리기
        DrawRect(
            BoxColor,
            ViewportCenter.X,
            ViewportCenter.Y - (LineThickness / 2.f),
            LineLength,
            LineThickness
        );
    }
}
