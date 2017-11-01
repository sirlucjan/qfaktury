/*
 * CustomPaymData.h
 *
 *  Created on: Apr 4, 2009
 *      Author: moux
 */

#include "detector.h"
#if _pragma_once_support
#pragma once
#endif
#ifndef CUSTOMPAYMDATA_H_
#define CUSTOMPAYMDATA_H_

#include <QDate>
#include <QString>

// class for holding data from splitting cost sum, when prepayment method is checked. One instance per one document
class CustomPaymData {

public:
  CustomPaymData();
  virtual ~CustomPaymData();

  QString payment1;
  QString payment2;
  QDate date1;
  QDate date2;
  double amount1;
  double amount2;
};

#endif /* CUSTOMPAYMDATA_H_ */
