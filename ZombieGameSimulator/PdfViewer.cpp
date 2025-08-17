#include "PdfViewer.h"
#include <iostream>
#include <string>
#include <fpdfview.h>
#include "Global.h"
#include "Log.h"
using namespace std;

#define PDF(name) ((Global::SYSTEM::USE_APPDATA ? (string)std::getenv("APPDATA") + "\\" + Global::SYSTEM::NAME + "\\" : (string)"") + (string)Global::SYSTEM::RESOURCE_PATH + name + Global::SYSTEM::PDF_EXTENSION).c_str()

PdfViewer::PdfViewer(std::string name, SDL_Renderer *ren) {
	std::filesystem::path p = PDF(name);
	std::string pUtf8 = p.u8string();
	this->pdf_document = FPDF_LoadDocument(pUtf8.c_str(), nullptr);
	if (!this->pdf_document) {
		Log::FormattedDebug("PdfViewer", "Constructor", "Failed to load PDF document: " + name + ".");
		return;
	}
	this->page_cnt = FPDF_GetPageCount(this->pdf_document);
	this->page_index = 0;

	this->renderer = ren;

	Log::FormattedDebug("PdfViewer", "Constructor", "Loaded PDF document: " + name + ", Page Count: " + std::to_string(this->page_cnt) + ".");
}

PdfViewer::~PdfViewer() {
	if (this->pdf_document) {
		FPDF_CloseDocument(this->pdf_document);
		this->pdf_document = nullptr;
		Log::FormattedDebug("PdfViewer", "Destructor", "Closed PDF document.");
	}
	else {
		Log::FormattedDebug("PdfViewer", "Destructor", "No PDF document to close.");
	}
}

int PdfViewer::GetPageCount() const {
	return this->page_cnt;
}

void PdfViewer::NextPage() {
	if (this->page_index < this->page_cnt - 1) {
		this->page_index++;
		Log::FormattedDebug("PdfViewer", "NextPage", "Moved to next page: " + std::to_string(this->page_index + 1) + ".");
	}
	else {
		Log::FormattedDebug("PdfViewer", "NextPage", "Already on the last page.");
	}
}

void PdfViewer::PreviousPage() {
	if (this->page_index > 0) {
		this->page_index--;
		Log::FormattedDebug("PdfViewer", "PreviousPage", "Moved to previous page: " + std::to_string(this->page_index + 1) + ".");
	}
	else {
		Log::FormattedDebug("PdfViewer", "PreviousPage", "Already on the first page.");
	}
}

bool PdfViewer::IsEnd() {
	return this->page_index >= this->page_cnt - 1;
}

bool PdfViewer::IsStart() {
	return this->page_index <= 0;
}

void PdfViewer::SetPage(int index) {
	if (index < 0 || index >= this->page_cnt) {
		Log::FormattedDebug("PdfViewer", "SetPage", "Invalid page index: " + std::to_string(index) + ". Must be between 0 and " + std::to_string(this->page_cnt - 1) + ".");
		return;
	}
	this->page_index = index;
	Log::FormattedDebug("PdfViewer", "SetPage", "Set current page to: " + std::to_string(this->page_index + 1) + ".");
}

int PdfViewer::GetCurrentPageIndex() const {
	if (!this->pdf_document) {
		Log::FormattedDebug("PdfViewer", "GetCurrentPageIndex", "No PDF document loaded.");
		return -1;
	}
	Log::FormattedDebug("PdfViewer", "GetCurrentPageIndex", "Current page index: " + std::to_string(this->page_index + 1) + ".");
	return this->page_index;
}

SDL_Texture* PdfViewer::GetCurrentPageTexture() {
	if (!this->pdf_document) {
		Log::FormattedDebug("PdfViewer", "GetCurrentPageTexture", "No PDF document loaded.");
		return nullptr;
	}
	FPDF_PAGE page = FPDF_LoadPage(this->pdf_document, this->page_index);
	if (!page) {
		Log::FormattedDebug("PdfViewer", "GetCurrentPageTexture", "Failed to load page: " + std::to_string(this->page_index + 1) + ".");
		return nullptr;
	}
	FPDF_BITMAP bitmap = FPDFBitmap_CreateEx(Global::WIN::SCREEN_WIDTH, Global::WIN::SCREEN_HEIGHT, FPDFBitmap_BGR, nullptr, 0);
	if (!bitmap) {
		Log::FormattedDebug("PdfViewer", "GetCurrentPageTexture", "Failed to create bitmap for page: " + std::to_string(this->page_index + 1) + ".");
		FPDF_ClosePage(page);
		return nullptr;
	}
	FPDF_RenderPageBitmap(bitmap, page, 0, 0, Global::WIN::SCREEN_WIDTH, Global::WIN::SCREEN_HEIGHT, 0, 0);
	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(FPDFBitmap_GetBuffer(bitmap), Global::WIN::SCREEN_WIDTH, Global::WIN::SCREEN_HEIGHT, 24, FPDFBitmap_GetStride(bitmap), 0x00FF0000, 0x0000FF00, 0x000000FF, 0);
	if (!surface) {
		Log::FormattedDebug("PdfViewer", "GetCurrentPageTexture", "Failed to create SDL surface for page: " + std::to_string(this->page_index + 1) + ".");
		FPDFBitmap_Destroy(bitmap);
		FPDF_ClosePage(page);
		return nullptr;
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(this->renderer, surface);
	FPDFBitmap_Destroy(bitmap);
	SDL_FreeSurface(surface);
	FPDF_ClosePage(page);
	if (!texture) {
		Log::FormattedDebug("PdfViewer", "GetCurrentPageTexture", "Failed to create SDL texture for page: " + std::to_string(this->page_index + 1) + ".");
		return nullptr;
	}
	Log::FormattedDebug("PdfViewer", "GetCurrentPageTexture", "Successfully created texture for page: " + std::to_string(this->page_index + 1) + ".");
	return texture;
}

