//----------------------------------------------------------------------------------------------------------------------
//	ImageHelper.cpp			©2025 Stevo Brock		All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "ImageHelper.h"

#include "winrt\Microsoft.UI.Dispatching.h"
#include "winrt\Microsoft.UI.Xaml.h"
#include "winrt\Microsoft.UI.Xaml.Media.Imaging.h"
#include "winrt\Windows.ApplicationModel.h"
#include "winrt\Windows.Foundation.h"
#include "winrt\Windows.Storage.h"
#include "winrt\Windows.Storage.Streams.h"
#include "winrt\Windows.System.Threading.h"

#include <fstream>
#include <iterator>
#include <map>
#include <string>

using BitmapImage = winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage;
using DataWriter = winrt::Windows::Storage::Streams::DataWriter;
using ElementTheme = winrt::Microsoft::UI::Xaml::ElementTheme;
using IAsyncAction = winrt::Windows::Foundation::IAsyncAction;
using InMemoryRandomAccessStream = winrt::Windows::Storage::Streams::InMemoryRandomAccessStream;
using Package = winrt::Windows::ApplicationModel::Package;
using Size = winrt::Windows::Foundation::Size;
using SvgImageSource = winrt::Microsoft::UI::Xaml::Media::Imaging::SvgImageSource;
using ThreadPool = winrt::Windows::System::Threading::ThreadPool;
using Uri = winrt::Windows::Foundation::Uri;

//----------------------------------------------------------------------------------------------------------------------
// MARK: Local Data

static std::map<std::wstring, Size>	sSVGSizeCache;

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - Local procs

//----------------------------------------------------------------------------------------------------------------------
static float readSVGAttribute(const std::string& tag, const std::string& name)
//----------------------------------------------------------------------------------------------------------------------
{
	// Find name="value"
	std::string	token = name + "=\"";
	size_t		start = tag.find(token);
	if (start == std::string::npos)
		return 0.0f;
	start += token.length();

	size_t	end = tag.find('"', start);
	if (end == std::string::npos)
		return 0.0f;

	return std::stof(tag.substr(start, end - start));
}

//----------------------------------------------------------------------------------------------------------------------
static std::wstring packagePathForURI(const winrt::hstring& uri)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	static	const	std::wstring	sInstallPath = Package::Current().InstalledLocation().Path().c_str();

	// Strip the "ms-appx:///" scheme, leaving the package-relative path
	std::wstring	path(uri.c_str());
	size_t			schemeEnd = path.find(L"///");
	if (schemeEnd != std::wstring::npos)
		// Adjust path
		path = path.substr(schemeEnd + 3);

	return sInstallPath + L"\\" + path;
}

//----------------------------------------------------------------------------------------------------------------------
static Size intrinsicSizeOfSVG(const winrt::hstring& uri)
//----------------------------------------------------------------------------------------------------------------------
{
	// Check cache
	std::wstring	key(uri.c_str());
	auto			cachedSize = sSVGSizeCache.find(key);
	if (cachedSize != sSVGSizeCache.end())
		return cachedSize->second;

	// Read the file
	std::ifstream	file(packagePathForURI(uri).c_str());
	std::string		content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	// Isolate the opening <svg ...> tag
	size_t		tagStart = content.find("<svg");
	size_t		tagEnd = (tagStart != std::string::npos) ? content.find('>', tagStart) : std::string::npos;
	std::string	tag = (tagEnd != std::string::npos) ? content.substr(tagStart, tagEnd - tagStart) : "";

	// Setup size and store
	Size	size{readSVGAttribute(tag, "width"), readSVGAttribute(tag, "height")};
	sSVGSizeCache[key] = size;

	return size;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// MARK: - ImageHelper

// MARK: Instance methods

//----------------------------------------------------------------------------------------------------------------------
ImageHelper& ImageHelper::setImageData(const IRandomAccessStream& randomAccessStream)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup Image source
	BitmapImage	bitmapImage;
	bitmapImage.SetSource(randomAccessStream);

	// Set image
	getImage().Source(bitmapImage);

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ImageHelper& ImageHelper::setImageData(const std::vector<uint8_t>& imageData)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	Image	image = getImage();

	// Run in background
	ThreadPool::RunAsync([imageData, image](const IAsyncAction& asyncAction){
		// Setup stream
		InMemoryRandomAccessStream	memoryStream;

		// Write image data to stream
		DataWriter	dataWriter(memoryStream.GetOutputStreamAt(0));
		dataWriter.WriteBytes(imageData);
		dataWriter.StoreAsync();
		dataWriter.FlushAsync();
		dataWriter.DetachStream();

		// Switch to UI thread
		image.DispatcherQueue().TryEnqueue([memoryStream, image](){
			// Set image
			ImageHelper(image).setImageData(memoryStream);
		});
	});

	return *this;
}

//----------------------------------------------------------------------------------------------------------------------
ImageHelper& ImageHelper::setImageData(const uint8_t* imageDataPtr, size_t byteCount)
//----------------------------------------------------------------------------------------------------------------------
{
	return setImageData(std::vector<uint8_t>(imageDataPtr, imageDataPtr + byteCount));
}

//----------------------------------------------------------------------------------------------------------------------
ImageHelper& ImageHelper::set(const ThemedSVG& themedSVG)
//----------------------------------------------------------------------------------------------------------------------
{
	// Setup
	Image	image = getImage();

	// Select the URI for the Image's current theme
	winrt::hstring	uri = (image.ActualTheme() == ElementTheme::Dark) ? themedSVG.mDarkURI : themedSVG.mLightURI;

	// Set Source
	image.Source(SvgImageSource(Uri(uri)));

	// Set size
	Size	size = intrinsicSizeOfSVG(uri);
	image.Width(size.Width);
	image.Height(size.Height);

	return *this;
}
