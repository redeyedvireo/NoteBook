
#ifndef uidef_h__
#define uidef_h__

typedef enum
{
	eNewPage = 0,
    eNewFolder,
    eNewToDoListPage
} PAGE_ADD;

/*
This enum describes various ways in which pages can be added.
The following rules apply for the "default" mode:
 - if there is no current item in the page tree, then add a new top-level item
 - if the current item is a page (not a folder), then add a sibling item
 - if the current item is a folder, then add a child
*/
typedef enum
{
	ePageAddDefault = 0,
	ePageAddAfterCurrentItem,
	ePageAddBeforeCurrentItem,
	ePageAddTopLevel
} PAGE_ADD_WHERE;

#endif // uidef_h__
