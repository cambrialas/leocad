#include "lc_global.h"
#include "lc_http.h"
#include <iostream>
#ifdef Q_OS_WIN

#include <windows.h>
#include <wininet.h>

lcHttpReply::lcHttpReply(QObject* Parent, const QString& URL)
	: QThread(Parent)
{
	mError = true;
	mAbort = false;
	mURL = URL;
}

void lcHttpReply::run()
{
	HINTERNET Session = nullptr;
	HINTERNET Request = nullptr;

	static_assert(sizeof(wchar_t) == sizeof(QChar), "Character size mismatch");

	Session = InternetOpen(L"LeoCAD", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (!Session)
		return;

	Request = InternetOpenUrl(Session, (WCHAR*)mURL.data(), NULL, 0, 0, 0);
	if (!Request)
	{
		InternetCloseHandle(Session);
		return;
	}

	for (;;)
	{
		char Buffer[1024];
		DWORD BytesRead;

		if (mAbort)
			break;

		if (!InternetReadFile(Request, Buffer, sizeof(Buffer), &BytesRead))
			break;

		if (BytesRead)
			mBuffer.append(Buffer, BytesRead);
		else
		{
			mError = false;
			break;
		}

		Sleep(0);
	}

	InternetCloseHandle(Request);
	InternetCloseHandle(Session);
}

lcHttpManager::lcHttpManager(QObject* Owner)
	: QObject(Owner)
{
}

lcHttpReply* lcHttpManager::DownloadFile(const QString& Url)
{
	lcHttpReply* Reply = new lcHttpReply(this, Url);
	connect(Reply, &QThread::finished, [this, Reply] { emit DownloadFinished(Reply); });
	Reply->start();
	return Reply;
}

#else

lcHttpManager::lcHttpManager(QObject* Owner)
	: QNetworkAccessManager(Owner)
{
	connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(Finished(QNetworkReply*)));
}

lcHttpReply* lcHttpManager::lcHttpManager::DownloadFile(const QString& Url)
{
    std::cout << "DownloadFile!" << Url.toStdString() << "\n";
    QNetworkRequest req = QNetworkRequest(QUrl(Url));
    //req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true); // Deprecated from Qt5.9, use QNetworkRequest::RedirectPolicyAttribute instead.
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    
	return (lcHttpReply*)get(req);
}

void lcHttpManager::Finished(QNetworkReply* Reply)
{
    std::cout << " ------------------------- DownloadFile! Finished -------------------------\n";
    std::cout << "Error code: " << Reply->error() << "\n";
    std::cout << "Data buffer size: " << Reply->readBufferSize() << "\n";
    std::cout << "Url: " << Reply->url().toString().toStdString() << "\n";

    QVariant statusCode = Reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );
    std::cout << "Status code: " << statusCode.toInt() << "\n";

    QList headerList = Reply->rawHeaderPairs();
    for(int i = 0; i < headerList.size(); i++) {
        QNetworkReply::RawHeaderPair headerPair = headerList.at(i);        
        std::cout << "Header: " <<  headerPair.first.toStdString() << " = " << headerPair.second.toStdString() << "\n";  
    }

    QVariant possibleRedirectUrl = Reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    
    if(possibleRedirectUrl.isValid()) {
        QUrl qurl = possibleRedirectUrl.toUrl();
        std::cout << "Redirect url: " << qurl.toString().toStdString() << "\n";
    }
    else {
        std::cout << "No redirect!\n";
    }

    /*QByteArray mData = Reply->readAll();
    std::cout << "\nContent:\n";
    std::cout << mData.toStdString() << "\n";*/
   
   
    /*QVariant possibleRedirectUrl = Reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    if(possibleRedirectUrl.isValid())
    {
        get(QNetworkRequest(possibleRedirectUrl.toUrl()));
        return;
    }*/

	emit DownloadFinished((lcHttpReply*)Reply);
}

#endif
