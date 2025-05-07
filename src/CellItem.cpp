#include "CellItem.h"

CellItem::CellItem( int _cellID,int doorBoardID_, int boardInterfaceID_, int rfidBoardID_, int rfidInterfaceID_):
cellID(_cellID),doorBoardID(doorBoardID_),boardInterfaceID(boardInterfaceID_),rfidBoardID(rfidBoardID_),rfidInterfaceID(rfidInterfaceID_)
{
}

void CellItem::SetRfHandle(RFIDReader rfid)
{
}

void CellItem::Refresh()
{
}

bool CellItem::ReadDoor()
{
    return false;
}

void CellItem::ReadRFID()
{
}

void CellItem::OpenDoor()
{
}

bool CellItem::CheckValid(uint8_t retryCount)
{
    return false;
}

bool CellItem::CheckGoodValid(const char *flowStatName)
{
    return false;
}

void CellItem::AddType(const char *type)
{
}

bool CellItem::isMyGood(const char *goodType)
{
    return false;
}
