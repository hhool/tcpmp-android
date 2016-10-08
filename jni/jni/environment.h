#ifndef __ENVIRONMENT_WRAP_H
#define __ENVIRONMENT_WRAP_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Environment
{
	int	  	m_iSdkVersion;
	tchar_t m_szCountry[256];
	int		m_LangId;
	tchar_t m_szModel[256];
	tchar_t m_szSysVersion[256];
}Environment;

extern Environment g_Environment;

enum
{
	ENUM_LANG_EN = 1,
	ENUM_LANG_CHS= 2
};
const int Lang_en = ENUM_LANG_EN;

typedef	int (*notify_draw_func)(void* This,int Param,int Param2);

typedef struct notify_draw
{
	notify_draw_func Func;
	void* This;

} notify_draw;
#ifdef __cplusplus
}
#endif
#endif
