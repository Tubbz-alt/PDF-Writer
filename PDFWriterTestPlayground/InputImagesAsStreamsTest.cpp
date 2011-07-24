#include "InputImagesAsStreamsTest.h"
#include "TestsRunner.h"
#include "InputFile.h"
#include "TestsRunner.h"
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PageContentContext.h"
#include "PDFFormXObject.h"

#include <string>
#include <iostream>
using namespace std;

InputImagesAsStreamsTest::InputImagesAsStreamsTest(void)
{
}

InputImagesAsStreamsTest::~InputImagesAsStreamsTest(void)
{
}


EStatusCode InputImagesAsStreamsTest::Run()
{
	// A minimal test to see if images as streams work. i'm using regular file streams, just to show the point
	// obviously this is quite a trivial case.

	PDFWriter pdfWriter;
	EStatusCode status; 

	do
	{
		status = pdfWriter.StartPDF(L"C:\\PDFLibTests\\ImagesInStreams.PDF",ePDFVersion13);
		if(status != eSuccess)
		{
			wcout<<"failed to start PDF\n";
			break;
		}	

		PDFPage* page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		// JPG image

		InputFile jpgImage;

		status = jpgImage.OpenFile(L"C:\\PDFLibTests\\TestMaterials\\images\\otherStage.JPG");
		if(status != eSuccess)
		{
			wcout<<"failed to open JPG image in"<<L"C:\\PDFLibTests\\TestMaterials\\images\\otherStage.JPG"<<"\n";
			break;
		}


		PDFFormXObject*  formXObject = pdfWriter.CreateFormXObjectFromJPGStream(jpgImage.GetInputStream());
		if(!formXObject)
		{
			wcout<<"failed to create form XObject from file\n";
			status = eFailure;
			break;
		}

		jpgImage.CloseFile();

		PageContentContext* pageContentContext = pdfWriter.StartPageContentContext(page);
		if(NULL == pageContentContext)
		{
			status = eFailure;
			wcout<<"failed to create content context for page\n";
		}

		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,0,400);
		pageContentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(formXObject->GetObjectID()));
		pageContentContext->Q();

		delete formXObject;

		status = pdfWriter.EndPageContentContext(pageContentContext);
		if(status != eSuccess)
		{
			wcout<<"failed to end page content context\n";
			break;
		}

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
		{
			wcout<<"failed to write page\n";
			break;
		}

		// TIFF image
		page = new PDFPage();
		page->SetMediaBox(PDFRectangle(0,0,595,842));

		InputFile tiffFile;
		status = tiffFile.OpenFile(L"C:\\PDFLibTests\\TestMaterials\\images\\tiff\\FLAG_T24.TIF");
		if(status != eSuccess)
		{
			wcout<<"failed to open TIFF image in"<<L"C:\\PDFLibTests\\TestMaterials\\images\\tiff\\FLAG_T24.TIF"<<"\n";
			break;
		}

		formXObject = pdfWriter.CreateFormXObjectFromTIFFStream(tiffFile.GetInputStream());
		if(!formXObject)
		{
			wcout<<"failed to create image form XObject for TIFF\n";
			status = eFailure;
			break;
		}

		tiffFile.CloseFile();

		pageContentContext = pdfWriter.StartPageContentContext(page);
		if(NULL == pageContentContext)
		{
			status = eFailure;
			wcout<<"failed to create content context for page with TIFF image\n";
		}

		// continue page drawing, place the image in 0,0 (playing...could avoid CM at all)
		pageContentContext->q();
		pageContentContext->cm(1,0,0,1,0,0);
		pageContentContext->Do(page->GetResourcesDictionary().AddFormXObjectMapping(formXObject->GetObjectID()));
		pageContentContext->Q();

		delete formXObject;

		status = pdfWriter.EndPageContentContext(pageContentContext);
		if(status != eSuccess)
		{
			wcout<<"failed to end page content context for TIFF\n";
			break;
		}

		status = pdfWriter.WritePageAndRelease(page);
		if(status != eSuccess)
		{
			wcout<<"failed to write page, for TIFF\n";
			break;
		}

		// PDF
		
		InputFile pdfFile;

		status = pdfFile.OpenFile(L"C:\\PDFLibTests\\TestMaterials\\Original.pdf");
		if(status != eSuccess)
		{
			wcout<<"failed to open PDF file in"<<L"C:\\PDFLibTests\\TestMaterials\\Original.pdf"<<"\n";
			break;
		}

		status = pdfWriter.AppendPDFPagesFromPDF(pdfFile.GetInputStream(),PDFPageRange()).first;
		if(status != eSuccess)
		{
			wcout<<"failed to append pages from Original.PDF\n";
			break;
		}

		pdfFile.CloseFile();

		status = pdfWriter.EndPDF();
		if(status != eSuccess)
		{
			wcout<<"failed in end PDF\n";
			break;
		}
	}while(false);
	return status;	
}


ADD_CATEGORIZED_TEST(InputImagesAsStreamsTest,"CustomStreams")