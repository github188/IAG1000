
/*-------------------------------------------------------------------------*/
/**
   @file    iniparser.h
   @author  N. Devillard
   @date    Mar 2000
   @version $Revision: 1.1.1.1 $
   @brief   Parser for ini files.
*/
/*--------------------------------------------------------------------------*/

#ifndef _INIPARSER_H_
#define _INIPARSER_H_

/*---------------------------------------------------------------------------
   								Includes
 ---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include "dictionary.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
	//windows ʹ��
	#define EXPORT_DLL __declspec(dllexport)
#else
	//linux ʹ��
	#define EXPORT_DLL
#endif


/*-------------------------------------------------------------------------*/
/**
  @brief    Get number of sections in a dictionary
  @param    d   Dictionary to examine
  @return   int Number of sections found in dictionary

  This function returns the number of sections found in a dictionary.
  The test to recognize sections is done on the string stored in the
  dictionary: a section name is given as "section" whereas a key is
  stored as "section:key", thus the test looks for entries that do not
  contain a colon.

  This clearly fails in the case a section name contains a colon, but
  this should simply be avoided.

  This function returns -1 in case of error.
 */
/*--------------------------------------------------------------------------*/
 /**********************************************************************************************
 * ������	:iniparser_getnsec()
 * ����	:��ȡini�ļ��е��ܽ���
 * ����	:d:����ini�ļ��Ľṹ
 * ����ֵ	:��ֵ��ʾini�ṹ�е��ܽ�������ֵ��ʾ����
 **********************************************************************************************/
EXPORT_DLL int iniparser_getnsec(dictionary * d);


/*-------------------------------------------------------------------------*/
/**
  @brief    Get name for section n in a dictionary.
  @param    d   Dictionary to examine
  @param    n   Section number (from 0 to nsec-1).
  @return   Pointer to char string

  This function locates the n-th section in a dictionary and returns
  its name as a pointer to a string statically allocated inside the
  dictionary. Do not free or modify the returned string!

  This function returns NULL in case of error.
 */
/*--------------------------------------------------------------------------*/
 /**********************************************************************************************
 * ������	:iniparser_getsecname()
 * ����	:��ȡini�ļ��еĵ�n���ڵĽ���
 * ����	:d:����ini�ļ��Ľṹ
 *			 n:Ҫȡ���Ľ����
 * ����ֵ	:ָ����ŵĽڵ����֣�NULL��ʾû�ҵ�(������)
 **********************************************************************************************/
EXPORT_DLL char * iniparser_getsecname(dictionary * d, int n);


/*-------------------------------------------------------------------------*/
/**
  @brief    Save a dictionary to a loadable ini file
  @param    d   Dictionary to dump
  @param    f   Opened file pointer to dump to
  @return   void

  This function dumps a given dictionary into a loadable ini file.
  It is Ok to specify @c stderr or @c stdout as output files.
 */
/*--------------------------------------------------------------------------*/
/**********************************************************************************************
 * ������	:iniparser_dump_ini()
 * ����	:��ini�ṹ�����һ���Ѿ��򿪵��ļ���(ini��ʽ)
 * ����	:d:����ini�ļ��Ľṹ
 * ���	:f:�Ѿ��򿪵��ļ�����ini�ṹ��������������
 * ����ֵ	:��
 **********************************************************************************************/
EXPORT_DLL void iniparser_dump_ini(dictionary * d, FILE * f);

/*-------------------------------------------------------------------------*/
/**
  @brief    Dump a dictionary to an opened file pointer.
  @param    d   Dictionary to dump.
  @param    f   Opened file pointer to dump to.
  @return   void

  This function prints out the contents of a dictionary, one element by
  line, onto the provided file pointer. It is OK to specify @c stderr
  or @c stdout as output files. This function is meant for debugging
  purposes mostly.
 */
/*--------------------------------------------------------------------------*/
/**********************************************************************************************
 * ������	:iniparser_dump()
 * ����	:��ini�ṹ�еı��������һ���Ѿ��򿪵��ļ���
 * ����	:d:����ini�ļ��Ľṹ
 * ���	:f:�Ѿ��򿪵��ļ�����ini�ṹ��������������
 * ����ֵ	:��
 * ע:		������������ļ���ʽΪ
 *			[section:key]=[val]
 *			���ļ����ܱ�iniparser_load����
 **********************************************************************************************/
EXPORT_DLL void iniparser_dump(dictionary * d, FILE * f);

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key, return NULL if not found
  @param    d   Dictionary to search
  @param    key Key string to look for
  @return   pointer to statically allocated character string, or NULL.

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  NULL is returned.
  The returned char pointer is pointing to a string allocated in
  the dictionary, do not free or modify it.

  This function is only provided for backwards compatibility with
  previous versions of iniparser. It is recommended to use
  iniparser_getstring() instead.
 */
/*--------------------------------------------------------------------------*/
EXPORT_DLL char * iniparser_getstr(dictionary * d, char * key);


/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key
  @param    d       Dictionary to search
  @param    key     Key string to look for
  @param    def     Default value to return if key not found.
  @return   pointer to statically allocated character string

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the pointer passed as 'def' is returned.
  The returned char pointer is pointing to a string allocated in
  the dictionary, do not free or modify it.
 */
/*--------------------------------------------------------------------------*/
/**********************************************************************************************
 * ������	:iniparser_getstring()
 * ����	:��ini�ṹ�л�ȡָ��key�ı����ַ���ֵ
 * ����	:d:����ini�ļ��Ľṹ
 *			 key:Ҫ���ʵı���key	section:key
 *			def:����Ҳ���ָ�������ķ���ֵ
 * ����ֵ	:�����ַ���ֵ,���û���ҵ��򷵻�def
 
 **********************************************************************************************/
EXPORT_DLL char * iniparser_getstring(dictionary * d, char * key, char * def);

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key, convert to an int
  @param    d Dictionary to search
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   integer

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.
 */
/*--------------------------------------------------------------------------*/
/**********************************************************************************************
 * ������	:iniparser_getint()
 * ����	:��ini�ṹ�л�ȡָ��key�ı�������ֵ
 * ����	:d:����ini�ļ��Ľṹ
 *			 key:Ҫ���ʵı���key	section:key
 *			notfound:����Ҳ���ָ�������ķ���ֵ
 * ����ֵ	:ָ������������ֵ,���û���ҵ��򷵻�notfound
 **********************************************************************************************/
EXPORT_DLL int iniparser_getint(dictionary * d, char * key, int notfound);

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key, convert to a double
  @param    d Dictionary to search
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   double

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.
 */
/*--------------------------------------------------------------------------*/
EXPORT_DLL double iniparser_getdouble(dictionary * d, char * key, double notfound);

/*-------------------------------------------------------------------------*/
/**
  @brief    Get the string associated to a key, convert to a boolean
  @param    d Dictionary to search
  @param    key Key string to look for
  @param    notfound Value to return in case of error
  @return   integer

  This function queries a dictionary for a key. A key as read from an
  ini file is given as "section:key". If the key cannot be found,
  the notfound value is returned.

  A true boolean is found if one of the following is matched:

  - A string starting with 'y'
  - A string starting with 'Y'
  - A string starting with 't'
  - A string starting with 'T'
  - A string starting with '1'

  A false boolean is found if one of the following is matched:

  - A string starting with 'n'
  - A string starting with 'N'
  - A string starting with 'f'
  - A string starting with 'F'
  - A string starting with '0'

  The notfound value returned if no boolean is identified, does not
  necessarily have to be 0 or 1.
 */
/*--------------------------------------------------------------------------*/
EXPORT_DLL int iniparser_getboolean(dictionary * d, char * key, int notfound);


/*-------------------------------------------------------------------------*/
/**
  @brief    Set an entry in a dictionary.
  @param    ini     Dictionary to modify.
  @param    entry   Entry to modify (entry name)
  @param    val     New value to associate to the entry.
  @return   int 0 if Ok, -1 otherwise.

  If the given entry can be found in the dictionary, it is modified to
  contain the provided value. If it cannot be found, -1 is returned.
  It is Ok to set val to NULL.
 */
/*--------------------------------------------------------------------------*/
/**********************************************************************************************
 * ������	:iniparser_setstr()
 * ����	:���ַ���ֵ���õ�ini�ṹ�еı���
 * ����	:d:����ini�ļ��Ľṹ
 *			entry:Ҫ���ʵı���key	section:key�����ֻ�н������ʾ�����ý�
 *			val:Ҫ���õ��ַ���ֵ
 * ����ֵ	:0��ʾ�ɹ�����ֵ��ʾʧ��
 * ע		:��������������򴴽��ýڣ���������������򴴽��ñ���
 **********************************************************************************************/
EXPORT_DLL int iniparser_setstr(dictionary * ini, char * entry, char * val);

/*-------------------------------------------------------------------------*/
/**
  @brief    Delete an entry in a dictionary
  @param    ini     Dictionary to modify
  @param    entry   Entry to delete (entry name)
  @return   void

  If the given entry can be found, it is deleted from the dictionary.
 */
/*--------------------------------------------------------------------------*/
/**********************************************************************************************
 * ������	:iniparser_unset()
 * ����	:��ini�ṹ��ɾ��ָ���Ľڻ����
 * ����	:d:����ini�ļ��Ľṹ
 *			entry:Ҫɾ���Ľڻ����
 * ����ֵ	:��
 **********************************************************************************************/
EXPORT_DLL void iniparser_unset(dictionary * ini, char * entry);

/*-------------------------------------------------------------------------*/
/**
  @brief    Finds out if a given entry exists in a dictionary
  @param    ini     Dictionary to search
  @param    entry   Name of the entry to look for
  @return   integer 1 if entry exists, 0 otherwise

  Finds out if a given entry exists in the dictionary. Since sections
  are stored as keys with NULL associated values, this is the only way
  of querying for the presence of sections in a dictionary.
 */
/*--------------------------------------------------------------------------*/
/**********************************************************************************************
 * ������	:iniparser_find_entry()
 * ����	:�����Ƿ���ָ���Ľڻ��������
 * ����	:d:����ini�ļ��Ľṹ
 *			entry:Ҫ���ҵĽڻ������
 * ����ֵ	:1��ʾ���� 0��ʾ������
 **********************************************************************************************/
EXPORT_DLL int iniparser_find_entry(dictionary * ini, char * entry) ;

/*-------------------------------------------------------------------------*/
/**
  @brief    Parse an ini file and return an allocated dictionary object
  @param    ininame Name of the ini file to read.
  @return   Pointer to newly allocated dictionary

  This is the parser for ini files. This function is called, providing
  the name of the file to be read. It returns a dictionary object that
  should not be accessed directly, but through accessor functions
  instead.

  The returned dictionary must be freed using iniparser_freedict().
 */
/*--------------------------------------------------------------------------*/
/**********************************************************************************************
 * ������	:iniparser_load()
 * ����	:��ȡָ����ini�ļ���ת��Ϊini���ݽṹ����ָ�뷵��
 * ����	:ininame:Ҫ��ȡ��ini�ļ���
 * ����ֵ	:����ini�ļ��Ľṹָ�룬NULL��ʾ����
 * ע:		���ָ���������Ҫ��iniparser_freedict�����ͷ�
 **********************************************************************************************/
EXPORT_DLL dictionary * iniparser_load(char * ininame);

/*-------------------------------------------------------------------------*/
/**
  @brief    Free all memory associated to an ini dictionary
  @param    d Dictionary to free
  @return   void

  Free all memory associated to an ini dictionary.
  It is mandatory to call this function before the dictionary object
  gets out of the current context.
 */
/*--------------------------------------------------------------------------*/
/**********************************************************************************************
 * ������	:iniparser_freedict()
 * ����	:�ͷ��Ѿ��򿪲��Ҳ���ʹ�õ�ini�ṹָ��
 * ����	:d:Ҫ�ͷŵ�ָ��(iniparser_load�ķ���ֵ)
 * ����ֵ	:��
 **********************************************************************************************/
EXPORT_DLL void iniparser_freedict(dictionary * d);



/*-------------------------------------------------------------------------*/
/**
  @brief    ����ini�ļ������ݽṹ������ס�ļ�
  @param    	ininame:Ҫ�򿪵������ļ���
  	    		wait:����������ļ��Ѿ�������������Ƿ���еȴ� 1��ʾ�ȴ� 0��ʾֱ���˳�
  	    		lockf:ָ�����ļ���ָ�룬�������Ὣ�򿪵����ļ�ָ������ָ��
  @return   dictionary�ṹ ,NULL��ʾ���� lockf�д�������ļ���ָ�룬save_inidict_file���õ�
  @���lockf!=NULL�Ļ�Ҫ�������save_inidict_file���д洢�����fclose(lockf)���йرգ����ָ���������Ҫ��iniparser_freedict�����ͷ�

**/
/*--------------------------------------------------------------------------*/

EXPORT_DLL dictionary * iniparser_load_lockfile(char * ininame,int wait,FILE**lockf);
/*-------------------------------------------------------------------------*/
/**
  @brief    ��ini���ݽṹ���µ������ļ���������
  @param    filename:Ҫ�洢�������ļ���
  	    	ini:�Ѿ����õ����ݽṹ
  	    lockf:���ļ�ָ�룬����iniparser_load_lockfileʱ����lockf��ֵ
  @return   0��ʾ�ɹ���ֵ��ʾ����
**/
/*--------------------------------------------------------------------------*/
EXPORT_DLL int save_inidict_file(char *filename,dictionary * ini,FILE**lockf);

/*-------------------------------------------------------------------------*/
/**
  @brief    �����ͱ�����ֵ����ָ�������ݽṹ�е�key
  @param    d:����ini�����ļ��Ľṹָ��
  	    	key:Ҫ�洢��key
  	    	val:Ҫ�洢��ֵ
  @return   0��ʾ�ɹ���ֵ��ʾ����
**/
/*--------------------------------------------------------------------------*/
EXPORT_DLL int iniparser_setint(dictionary * d, char * key, int val);
/*-------------------------------------------------------------------------*/
/**
  @brief    �����ͱ�����ֵ��16���ƴ���ָ�������ݽṹ�е�key
  @param    	ini:����ini�����ļ��Ľṹָ��
  	    		section:Ҫ�洢��key
  	    		val:Ҫ�洢��ֵ
  @return   0��ʾ�ɹ���ֵ��ʾ����
**/
/*--------------------------------------------------------------------------*/
EXPORT_DLL int iniparser_sethex(dictionary *ini,char* section,int val);

/*-------------------------------------------------------------------------*/
/**
  @brief    ����ini�ļ����Ƿ���ָ���Ľ�,���û���򴴽�
  @param    filename:�����ļ���
  	    section:����
  @return   0��ʾ�ɹ���ֵ��ʾ����
**/
/*--------------------------------------------------------------------------*/
EXPORT_DLL int iniparser_find_creat_sec(char *filename,char*section);

/*-------------------------------------------------------------------------*/
/**
  @brief    ��ָ���������ַ������������ļ���
  @param    filename:�ļ���
  	    	    section:"����:������"
  	    	    vstr:�������ַ�����ʽ��ֵ  	   
  @return   0��ʾ�ɹ���ֵ��ʾ����
**/
/*--------------------------------------------------------------------------*/
EXPORT_DLL int save2para_file(char *filename,char *section,char *vstr);

/*-------------------------------------------------------------------------*/
/**
  @brief    ��ָ���������ַ�������ini���ݽṹ
  @param    ini:ini�ļ��ṹָ��
  	    	    section:"����:������"
  	    	    vstr:�������ַ�����ʽ��ֵ  	   
  @return   0��ʾ�ɹ���ֵ��ʾ����
**/
/*--------------------------------------------------------------------------*/
EXPORT_DLL int save2para(dictionary      *ini,char *section,char *vstr);

/******************************************************************
 * ������	ini_diff()
 * ����:	�Ƚ�����ini�ļ��Ƿ���ͬ
 * ����:	oldfile,newfile,����ini�ļ���
 * ����ֵ:  �����ļ����ʱ����0������ʱ����1,�������󷵻�-1
 *
 * ��������ļ���ͬ�����ն˺���־�ϼ�¼��Ϣ:
 * alarm:snap_pic_num 5->4				��ʾ����alarm:snap_pic_numԭ����5����ֵ��4
 * alarm:snap_pic_interval NULL->500	��ʾ����alarm: snap_pic_intervalԭ��û�У���ֵ��500
 * port:telnet_port 23->NULL		    ��ʾ����port: telnet_portԭ����23���������������ɾ����
******************************************************************/

EXPORT_DLL int ini_diff(char *oldfile,char *newfile);

/******************************************************************
 *  * ������	ini_set_file()
 *   * ����:	��source�ļ���ÿ���ڶ�������target�ļ���Ӧ�ڶ������ݱȽϣ�����ͬ���дtarget,��target�����ڸý��򴴽�,������־
 *    * ����:	source:Դ�ļ���
 *     *			target,����д��ini�ļ���
 *      * ����ֵ: 0��ʾ�ɹ�,��ֵ��ʾʧ��
 *       * 
 *        ******************************************************************/
EXPORT_DLL int ini_set_file(char *source,char *target);

#ifdef __cplusplus
}
#endif

#endif


