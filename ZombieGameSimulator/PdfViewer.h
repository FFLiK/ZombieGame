#pragma once
#include <fpdfview.h>
#include <string>
#include <SDL.h>

class PdfViewer {
private:
	FPDF_DOCUMENT pdf_document;
	int page_cnt;
	int page_index;
	SDL_Renderer* renderer;

public:
	PdfViewer(std::string name, SDL_Renderer *ren);
	~PdfViewer();

	int GetPageCount() const;
	void NextPage();
	void PreviousPage();
	bool IsEnd();
	bool IsStart();
	void SetPage(int index);
	int GetCurrentPageIndex() const;

	SDL_Texture* GetCurrentPageTexture();
};

