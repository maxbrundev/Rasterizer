#pragma once
#ifndef _EXPORT_H
#define _EXPORT_H

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

#ifdef AMBERGL_EXPORT
#define API_AMBERGL __declspec(dllexport)
#else
#define API_AMBERGL __declspec(dllimport)
#endif // AMBERGL_EXPORT

#endif // _EXPORT_H