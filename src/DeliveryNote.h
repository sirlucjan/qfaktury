#include "detector.h"
#if _pragma_once_support
#pragma once
#endif
#ifndef DELIVERYNOTE_H
#define DELIVERYNOTE_H

#include "Warehouse.h"
#include "WarehouseData.h"

// class for creating delivery note document, that belongs to warehouse
// documents
class DeliveryNote : public Warehouse {

public:
  DeliveryNote(QWidget *parent, IDataLayer *dl, QString in_form);
  virtual ~DeliveryNote();

  void readWarehouseData(QString fraFile);
  void setData(WarehouseData &invData);
  void getData(WarehouseData invData);
  void readData(QString fraFile);
  void setIsEditAllowed(bool isAllowed);

protected:
  void makeInvoiceSummAll();
  void makeInvoiceSumm();
  void makeInvoiceProductsHeadar();
  void makeInvoiceProducts(); // changed invoice form to delivery note form of
                              // products table
  void makeInvoiceFooter();
  void setData(InvoiceData &invData);
  using Invoice::getData;

private:
  bool ifcreateInv;
  WarehouseData *wareData;

private slots:

public slots:
  bool saveInvoice();
  void addGoods();
  void editGoods();
  void delGoods();
};

#endif // DELIVERYNOTE_H
