#include "Korekta.moc"
#include <qdatetime.h>
#include <qdir.h>
#include <qprocess.h>
#include <qapplication.h>
#include "Settings.h"
#include <Qt/qdom.h>
#include <qmessagebox.h>
#include <qtextcodec.h>
#include <QTextStream>
#include <QtDebug>


#include "KontrahenciLista.h"
#include "TowaryLista.h"
#include "ZmienIlosc.h"
#include "slownie.h"


// QDate liabDate;
int citem;


void Korekta::init ()
{
  // invoice fra1;
  // dodac do sett()ow!!

  tableTow->setColumnWidth (0, 30);	//index
  tableTow->setColumnWidth (1, 90);	// nazwa
  tableTow->setColumnWidth (2, 35);	// kod
  tableTow->setColumnWidth (3, 35);	// kod
  tableTow->setColumnWidth (4, 40);	// ilosc
  tableTow->setColumnWidth (5, 30);	// jm
  tableTow->setColumnWidth (6, 60);	// cena jedn.
  tableTow->setColumnWidth (7, 60);	// netto
  tableTow->setColumnWidth (8, 30);	// vat
  tableTow->setColumnWidth (9, 50);	// netto *  (wzor vat)
  // tableTow->setColumnWidth(5, 90);
  // sellingDate->setDate (QDate::currentDate ());
  sellingDate2->setDate (QDate::currentDate ());
  liabDate->setDate( QDate::currentDate() );

 platCombo->addItems( sett().value("payments").toString().split("|"));

 reasonCombo->addItems(sett().value("pkorekty").toString().split("|"));

 // connects
 QObject::connect(cancelbtn, SIGNAL(clicked()), this, SLOT(reject()));
 QObject::connect(saveBtn, SIGNAL(clicked()), this, SLOT(saveInvoice()));
 QObject::connect(backBtn, SIGNAL(clicked()), this, SLOT(backBtnClick()));
 QObject::connect(addTw, SIGNAL(clicked()), this, SLOT(addTow()));
 QObject::connect(editTw, SIGNAL(clicked()), this, SLOT(editTowar()));
 QObject::connect(rmTow, SIGNAL(clicked()), this, SLOT(delTowar()));
 QObject::connect(printBtn, SIGNAL(clicked()), this, SLOT(makeInvoice()));
 QObject::connect(rabatValue, SIGNAL(valueChanged(QString)), this, SLOT(rabatChange()));
 QObject::connect(tableTow, SIGNAL(doubleClicked(int,int,int,QPoint)), this, SLOT(editTowar()));
}


void Korekta::readData(QString fraFile)
{
  backBtn->setEnabled(false); // do this another way
  korNr->setEnabled(false);
  setWindowTitle ("Edytuje korekty");
  fName = fraFile;

  // here we would read all data from one xml file to the this window
  QDomDocument doc ("faktury");
  QDomElement root;
  QDomElement nabywca;
  QDomElement towary;
  QDomElement towaryOld;

  fName = fraFile;
  QFile file (progDir2 + "/faktury/" + fraFile);
  // qDebug( file.name() );
  if (!file.open (QIODevice::ReadOnly))
    {
      qDebug ("file doesn't exists");
      return;
    }
  else
    {
      QTextStream stream (&file);
      if (!doc.setContent (stream.readAll ()))
	{
	  file.close ();
	  // return;
	}
    }

  root = doc.documentElement ();
  frNr->setText (root.attribute ("nr"));
  sellingDate->
    setDate (QDate::fromString (root.attribute ("data.sprzed"), Qt::ISODate));
  sellingDate2->
    setDate (QDate::fromString (root.attribute ("data.korekty"), Qt::ISODate));

  QDomNode tmp;
  tmp = root.firstChild ();
  tmp = tmp.toElement ().nextSibling ();	// nabywca
  nabywca = tmp.toElement ();
  kontrName->setText (nabywca.attribute ("nazwa") + "," +
		      nabywca.attribute ("miasto") + "," +
		      nabywca.attribute ("ulica") + ", " +
		      nabywca.attribute ("nip"));
  kontrName->setCursorPosition (1);

  tmp = tmp.toElement ().nextSibling ();	// towary
  towary = tmp.toElement ();
  rabatValue->setValue (towary.attribute ("goods::rabat").toInt ());
  int towCount = towary.attribute ("goods::count").toInt ();
  int i = 0;
  QDomElement towar;
  towar = towary.firstChild ().toElement ();
  // qDebug( towar.attribute("Nazwa")  );
  tableTow->setRowCount(towCount);
  tableTow->setColumnCount(10);
  const char *towarColumns[] = {
      "",
"Nazwa",
"Kod",
"PKWiU",
"Ilosc",
"Jm.",
"Cena_jdn.",
"Wartosc_Netto",
"Stawka_VAT",
"Wartosc_Brutto",
  };
  for (i = 0; i < towCount; ++i)
    {
        for (int j = 0; j < int(sizeof(towarColumns)/sizeof(*towarColumns)); j++) {
            tableTow->setItem(i, j, new QTableWidgetItem(towar.attribute(towarColumns[j])));
        }
      towar = towar.nextSibling ().toElement ();
    }

 tmp = tmp.toElement ().nextSibling ();
towaryOld = tmp.toElement ();
// qDebug(  towaryOld.tagName() );
  QString tmp1;
  towCount = towaryOld.attribute ("goods::count").toInt ();
  // qDebug( towaryOld.attribute ("goods::count") );
  i = 0;
  towary = towaryOld.firstChild ().toElement ();

  oldNetto = 0;
  oldVat = 0;
  oldBrutto = 0;

  for (i = 0; i < towCount; ++i)
    {
      tmp1 = sett().numberToString(i);
      tmp1 += "|" + towary.attribute ("Nazwa");
      tmp1 += "|" + towary.attribute ("Kod");
      tmp1 += "|" + towary.attribute ("PKWiU");
      tmp1 += "|" + towary.attribute ("Ilosc");
      tmp1 += "|" + towary.attribute ("Jm.");
      tmp1 += "|" + towary.attribute ("Cena_jdn.");
      tmp1 += "|" + towary.attribute ("Wartosc_Netto");
      oldNetto += towary.attribute ("Wartosc_Netto").replace(",", ".").toDouble();
      tmp1 += "|" + towary.attribute ("Stawka_VAT");
      oldVat += towary.attribute ("Stawka_VAT").replace(",", ".").toDouble();
      tmp1 += "|" + towary.attribute ("Kwota_Vat");
      tmp1 += "|" + towary.attribute ("Wartosc_Brutto");
      oldBrutto += towary.attribute ("Wartosc_Brutto").replace(",", ".").toDouble();
      towary = towary.nextSibling ().toElement ();
      towaryPKor[i] = tmp1;
    }


  tmp = tmp.toElement ().nextSibling ();
  QDomElement additional = tmp.toElement ();
  additEdit->setText (additional.attribute ("text"));

  platCombo->setCurrentIndex(additional.attribute ("forma.plat").toInt ());

  reasonCombo->setCurrentIndex(additional.attribute ("przyczyna").toInt ());
//  addinfo.setAttribute ("przyczyna",
//			sett().numberToString(reasonCombo->currentItem ()));

//   qDebug( additional.attribute ("przyczyna") );
//   qDebug( additional.attribute ("forma.plat") );

  paymFry->setText( platCombo->currentText() );
  liabDate->
    setDate (QDate::
	     fromString (additional.attribute ("liabDate"), Qt::ISODate));
  citem = additional.attribute ("waluta").toInt ();
  // currCombo->setCurrentItem (additional.attribute ("waluta").toInt ());
  QStringList waluty = sett().value("elinux/faktury/waluty").toString().split("|");
  currency = waluty[additional.attribute ("waluta").toInt ()];
  // qDebug(currency); qFatal("dsadsa");
  fraValLbl->setText( sett().numberToString( oldBrutto, 'f', 2 ) );


  lastInvoice += "/" + QDate::currentDate ().toString ("dd");

    if ( !sett().value/*bool*/ ("elinux/faktury/edit") .toBool())
  {
      korNr->setEnabled( FALSE );
      backBtn->setEnabled( FALSE );
      sellingDate2->setEnabled( FALSE );
tableTow->setEnabled( FALSE );
rabatValue->setEnabled( FALSE );
platCombo->setEnabled( FALSE );
liabDate->setEnabled( FALSE );
reasonCombo->setEnabled( FALSE );
additEdit->setEnabled( FALSE );
addTw->setEnabled( FALSE );
rmTow->setEnabled( FALSE );
editTw->setEnabled( FALSE );
saveBtn->setEnabled( FALSE );
  } else {
      korNr->setEnabled( TRUE );
      backBtn->setEnabled( TRUE );
      sellingDate2->setEnabled( TRUE );
tableTow->setEnabled( TRUE );
rabatValue->setEnabled( TRUE );
platCombo->setEnabled( TRUE );
liabDate->setEnabled( TRUE );
reasonCombo->setEnabled( TRUE );
additEdit->setEnabled( TRUE );
addTw->setEnabled( TRUE );
rmTow->setEnabled( TRUE );
editTw->setEnabled( TRUE );
saveBtn->setEnabled( TRUE );
  }


  countRabat ();
  countSum ();


}

void Korekta::readDataNewKor (QString fraFile)
{
  // here we would read all data from one xml file to the this window
  QDomDocument doc ("faktury");
  QDomElement root;
  QDomElement nabywca;
  QDomElement towary;

  backBtnClick();

  fName = fraFile;
  QFile file (progDir2 + "/faktury/" + fraFile);
  // qDebug( file.name() );
  if (!file.open (QIODevice::ReadOnly))
    {
      qDebug ("file doesn't exists");
      return;
    }
  else
    {
      QTextStream stream (&file);
      if (!doc.setContent (stream.readAll ()))
	{
	  file.close ();
	  // return;
	}
    }

  root = doc.documentElement ();
  frNr->setText (root.attribute ("nr"));
  sellingDate->
    setDate (QDate::fromString (root.attribute ("data.sprzed"), Qt::ISODate));

  QDomNode tmp;
  tmp = root.firstChild ();
  tmp = tmp.toElement ().nextSibling ();	// nabywca
  nabywca = tmp.toElement ();
  kontrName->setText (nabywca.attribute ("nazwa") + "," +
		      nabywca.attribute ("miasto") + "," +
		      nabywca.attribute ("ulica") + ", " +
		      nabywca.attribute ("nip"));
  kontrName->setCursorPosition (1);

  tmp = tmp.toElement ().nextSibling ();	// towary
  towary = tmp.toElement ();
  rabatValue->setValue (towary.attribute ("goods::rabat").toInt ());
  int towCount = towary.attribute ("goods::count").toInt ();
  int i = 0;
  QDomElement towar;
  towar = towary.firstChild ().toElement ();
  // qDebug( towar.attribute("Nazwa")  );
  QString tmp1;
  oldNetto = 0;
  oldVat = 0;
  oldBrutto = 0;
  tableTow->setRowCount(towCount);
  tableTow->setColumnCount(10);
  for (i = 0; i < towCount; ++i)
    {
      tableTow->item (i, 0)->setText(sett().numberToString(i));	// lp
      tmp1 = sett().numberToString(i);

      tableTow->item (i, 1)->setText(towar.attribute ("Nazwa"));	// nazwa
      tmp1 += "|" + towar.attribute ("Nazwa");

      tableTow->item (i, 2)->setText(towar.attribute ("Kod"));	// kod
      tmp1 += "|" + towar.attribute ("Kod");

      tableTow->item (i, 3)->setText(towar.attribute ("PKWiU"));	// pkwiu
      tmp1 += "|" + towar.attribute ("PKWiU");

      tableTow->item (i, 4)->setText(towar.attribute ("Ilosc"));	// ilosc
      tmp1 += "|" + towar.attribute ("Ilosc");

      tableTow->item (i, 5)->setText(towar.attribute ("Jm."));	// jm
      tmp1 += "|" + towar.attribute ("Jm.");

      tableTow->item (i, 6)->setText(towar.attribute ("Cena_jdn.")); // cena jdn.
      tmp1 += "|" + towar.attribute ("Cena_jdn.");

      tableTow->item (i, 7)->setText(towar.attribute ("Wartosc_Netto")); // netto
      tmp1 += "|" + towar.attribute ("Wartosc_Netto");
      oldNetto += towar.attribute ("Wartosc_Netto").replace(",", ".").toDouble();

      tableTow->item (i, 8)->setText(towar.attribute ("Stawka_VAT"));	// vat
      tmp1 += "|" + towar.attribute ("Stawka_VAT");
      oldVat += towar.attribute ("Stawka_VAT").replace(",", ".").toDouble();


      tableTow->item (i, 9)->setText(towar.attribute ("Wartosc_Brutto")); // brutto
      tmp1 += "|" + towar.attribute ("Kwota_Vat");
      tmp1 += "|" + towar.attribute ("Wartosc_Brutto");
      oldBrutto += towar.attribute ("Wartosc_Brutto").replace(",", ".").toDouble();

      towar = towar.nextSibling ().toElement ();
      towaryPKor[i] = tmp1;
    }
  tmp = tmp.toElement ().nextSibling ();
  QDomElement additional = tmp.toElement ();
  additEdit->setText (additional.attribute ("text"));

  platCombo->setCurrentIndex(additional.attribute ("forma.plat").toInt ());
  paymFry->setText( platCombo->currentText() );
 liabDate->setDate( QDate::currentDate() );
/*
  liabDate->
    setDate (QDate::
	     fromString (additional.attribute ("liabDate"), Qt::ISODate));
  */
  citem = additional.attribute ("waluta").toInt ();

// currCombo->setCurrentItem (additional.attribute ("waluta").toInt ());
  QStringList waluty = sett().value("waluty").toString().split("|");
  currency = waluty[additional.attribute ("waluta").toInt ()];
  // qDebug(currency); qFatal("dsadsa");
  fraValLbl->setText( sett().numberToString( oldBrutto, 'f', 2 ) );

  if ( sett().value/*bool*/ ("elinux/faktury/editSymbol") .toBool())
  {
      korNr->setEnabled( FALSE );
      backBtn->setEnabled( FALSE );
  } else {
      korNr->setEnabled( TRUE );
      backBtn->setEnabled( TRUE );
  }


  countRabat ();
  countSum ();
}

void Korekta::getKontrahent ()
{
  KontrahenciLista *klWindow =
    new KontrahenciLista(this);
  if (klWindow->exec () == QDialog::Accepted)
    {
      kontrName->setText (klWindow->ret);
      kontrName->setCursorPosition (1);
    }
}

void Korekta::addTow ()
{
  TowaryLista *twWindow =
    new TowaryLista(this);
  if (twWindow->exec () == QDialog::Accepted)
    {
      // qDebug( twWindow->ret );
      // DAR|100|0,12|22|14.64
      // twarers|21398edwa|45|szt.|15,45|22|848.205
      tableTow->insertRow (tableTow->rowCount());
      QStringList row = twWindow->ret.split("|");
      tableTow->item (tableTow->rowCount () - 1, 0)->setText(sett().numberToString(tableTow->rowCount ()));	// lp
      tableTow->item (tableTow->rowCount () - 1, 1)->setText(row[0]);	// nazwa
      tableTow->item (tableTow->rowCount () - 1, 2)->setText(row[1]);	// kod
      tableTow->item (tableTow->rowCount () - 1, 3)->setText(row[2]);	// pkwiu
      tableTow->item (tableTow->rowCount () - 1, 4)->setText(row[3]);	// ilosc
      tableTow->item (tableTow->rowCount () - 1, 5)->setText(row[4]);	// jm
      tableTow->item (tableTow->rowCount () - 1, 6)->setText(row[5]);	// cena jdn.
      tableTow->item (tableTow->rowCount () - 1, 7)->setText(row[6]);	// netto
      tableTow->item (tableTow->rowCount () - 1, 8)->setText(row[7]);	// vat
      tableTow->item (tableTow->rowCount () - 1, 9)->setText(row[8]);	// brutto
    }
  countRabat ();
  countSum ();
}


void Korekta::countRabat ()
{
  QString rabat1 = sett().numberToString(rabatValue->value ());
  if (rabat1.length () == 1)
    rabat1 = "0.0" + rabat1;
  else
    rabat1 = "0." + rabat1;

  double kwota = 0;
  double cenajdn = 0;
  double rabat = 0;
  priceBRabN = 0;
  for (int i = 0; i < tableTow->rowCount (); ++i)
    {
      cenajdn = tableTow->item(i, 6)->text().toDouble ();
      kwota = cenajdn * tableTow->item(i, 4)->text().toInt ();
      rabat = kwota * rabat1.toDouble ();
      tableTow->item(i, 7)->setText(sett().numberToString(kwota - rabat, 'f', 2));	// netto
      priceBRabN = priceBRabN + kwota;
      // qDebug(tableTow->text(i, 6).replace(",", "."));
    }

  double vat = 0;
  double netto = 0;
  QString vat1;
  kwota = 0;
  cenajdn = 0;
  rabat = 0;
  priceBRab = 0;		// priceBRabN
/*
    rabat1 = sett().numberToString( rabatValue->value());
    if (rabat1.length() == 1) rabat1 = "1.0" + rabat1;
     else rabat1 = "1." + rabat1;
*/
  for (int i = 0; i < tableTow->rowCount (); ++i)
    {
      vat1 = tableTow->item(i, 8)->text().replace (",", ".");
      // sett().numberToString( rabatValue->value());
      if (vat1.length () == 1)
	vat1 = "1.0" + vat1;
      else
	vat1 = "1." + vat1;

      vat = vat1.toDouble ();
      if ( vat == 0 ) vat = 1;

      netto = tableTow->item(i, 7)->text().toDouble ();	// after rab.

//************************************************************************//

      cenajdn = tableTow->item(i, 6)->text().toDouble ();
      kwota = cenajdn * tableTow->item(i, 4)->text().toInt ();
      rabat = kwota * rabat1.toDouble ();
      // kwota - rabat; // jedn. netto
      priceBRab = priceBRab + (vat * (kwota));

//************************************************************************//

      kwota = vat * netto;
      // qDebug(sett().numberToString(kwota));
      tableTow->item(i, 9)->setText(sett().numberToString(kwota, 'f', 2));
    }

}


void Korekta::countSum ()
{
  // count netto sum
  double kwota = 0;
  for (int i = 0; i < tableTow->rowCount (); ++i)
    {
      kwota += tableTow->item(i, 7)->text().toDouble ();
      // qDebug(tableTow->text(i, 6).replace(",", "."));
    }
  snetto = sett().numberToString(kwota, 'f', 2);

  // count brutto sum
  kwota = 0;
  for (int i = 0; i < tableTow->rowCount (); ++i)
    {
      kwota += tableTow->item(i, 9)->text().toDouble ();
      // qDebug(tableTow->text(i, 6).replace(",", "."));
    }
  sbrutto = sett().numberToString(kwota, 'f', 2);

  // count rabat
  /* Not used!?
  QString rabat1 = sett().numberToString(rabatValue->value ());
  if (rabat1.length () == 1)
    rabat1 = "1.0" + rabat1;
  else
    rabat1 = "1." + rabat1;
    */
  // qDebug( "rabat " +  rabat1 );
  // double rabatVal = rabat1.toDouble();
  // double rabatBr = sbrutto->text().toDouble();
  // double przedRab = rabatVal * rabatBr;
  // qDebug("przed rabatem1: " + sett().numberToString(priceBRab));
  // qDebug("przed rabatem2: " + sett().numberToString(przedRab));
  sRabat = sett().numberToString(priceBRab - sbrutto.toDouble (), 'f', 2);
  // setting labels text

  fvKor->setText( sbrutto );
  diffLabel->setText( sett().numberToString( (oldBrutto - sbrutto.toDouble()) * -1, 'f', 2 )  );
}

void Korekta::rabatChange ()
{
  countRabat ();
  countSum ();
}

void Korekta::delTowar ()
{
  tableTow->removeRow (tableTow->currentRow ());
  for (int i = 0; i < tableTow->rowCount (); ++i)
    {
      tableTow->item(i, 0)->setText(sett().numberToString(i + 1));
    }
  countRabat ();
  countSum ();
}

void Korekta::editTowar ()
{
  // we can only modify amount
  ZmienIlosc *changeAmount =
    new ZmienIlosc(this);
  changeAmount->nameTow->setText (tableTow->
				  item(tableTow->currentRow (), 1)->text());
  changeAmount->codeTow->setText (tableTow->
				  item (tableTow->currentRow (), 2)->text());
  changeAmount->spinAmount->setValue (tableTow->
				      item (tableTow->currentRow (),
					    4)->text().toInt ());
  if (changeAmount->exec () == QDialog::Accepted)
    {
      // kontrName->setText( klWindow->ret );
      tableTow->item(tableTow->currentRow (), 4)->setText(
			 sett().numberToString(changeAmount->spinAmount->
					  value ()));
    }
  countRabat ();
  countSum ();
}




void Korekta::makeInvoiceHeadar ()
{

  fraStrList += "<html><head>";
  fraStrList +=
    "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-2\" />";
  fraStrList += "<meta name=\"creator\" value=\"http://www.e-linux.pl\" />";
  fraStrList += "</head>";
  fraStrList += "<title>______Korekta______</title>";
  fraStrList += "<style type=\"text/css\"><!-- ";
  // qDebug( templDir  );

  QFile file (templDir + "style.css");
  if (file.open (QIODevice::ReadOnly))
    {
      QTextStream stream (&file);
      QString line;
      while (!stream.atEnd ())
	{
	  line = stream.readLine ();
	  fraStrList += line;
	}
      file.close ();
    }

  fraStrList += "--></style>";
  fraStrList += "<body>";
  fraStrList +=
    "<table width=\"700\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">";
  //  class=\"page_break\" ------>>>> think about this
  fraStrList += "<tr comment=\"headar\"><td>";
  fraStrList +=
    "<table comment=\"headar table\" width=\"100%\" border=\"0\">";
  fraStrList += "<tr>";
  fraStrList +=
    "<td colspan=\"2\" width=\"60%\" align=\"left\" valign=\"center\" class=\"podpisy\">";
  // logo code
  // eof logo

  QString logo = sett().value ("elinux/faktury/logo").toString();
  if ( logo != "" ) {
      fraStrList += "<img src=\"" + logo + "\"  width=\"200\" height=\"100\">";
  } else {
      fraStrList += "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Piecz�� wystawcy";
  }
  // in case where we don't have a logo :(

  fraStrList += "</td>";
  fraStrList += "<td>";
  fraStrList += "<h2>FAKTURA VAT<br>koryguj�ca<br>";
  fraStrList += "NR: " + korNr->text () + "<br></h2>";
  fraStrList +=
    "<h5>Data wystawienia: " + sellingDate2->date ().toString ("yyyy-MM-dd") +
    "<br>";
  // fraStrList +=
  // QDate::currentDate ().toString ("yyyy-MM-dd")
  //   "Data sprzeda�y: " + sellingDate->date ().toString ("yyyy-MM-dd") +
  //    "<br></h5>";
  fraStrList += "</td>";
  fraStrList += "<td width=\"20\">&nbsp;</td>";
  fraStrList += "</tr>";
  fraStrList += "<tr>";
  fraStrList += "<td colspan=\"3\" align=\"right\" valign=\"top\">";
  fraStrList += "<br>ORYGINA�/KOPIA<br>";
  fraStrList += "</td>";
  fraStrList += "<td width=\"20\">&nbsp;</td>";
  fraStrList += "</tr>";
  fraStrList += "</table>";
  fraStrList += "</td></tr>";
}

void Korekta::makeInvoiceBody ()
{
  fraStrList += "<tr comment=\"body\"><td>";
  fraStrList += "<table width=\"100%\" border=\"0\">";
  fraStrList += "<tr>";
  fraStrList += "<td witdh=\"20\">&nbsp;</td>";
  fraStrList += "<td width=\"48%\"> ";
  fraStrList += "<h4>Sprzedawca:</h4>";

  fraStrList += "<h5>" + sett().value ("przelewy/user/nazwa").toString() + "<br>";
  fraStrList +=
    sett().value ("przelewy/user/kod").toString() + " " +
    sett().value ("przelewy/user/miejscowosc").toString() + "<br>";
  fraStrList += "ul. " + sett().value ("przelewy/user/adres").toString() + "<br>";
  fraStrList += "NIP: " + sett().value ("przelewy/user/nip").toString() + "<br>";
  fraStrList +=
    "Nr konta: " + sett().value ("przelewy/user/konto").toString().replace ("-",
								       " ") +
    "<br>";
  fraStrList += "</h5>";
  fraStrList += "</td>";
  fraStrList += "<td witdh=\"20\">&nbsp;</td>";
  fraStrList += "<td width=\"48%\">";
  fraStrList += "<h4>Nabywca:</h4>";
  fraStrList += "<h5>" + kontrName->text ().replace (",", "<br>") + "<br>";
  fraStrList += "</h5>";
  fraStrList += "</td>";
  fraStrList += "</tr>";
  fraStrList += "</table>";
  fraStrList += "</td></tr>";
}

void Korekta::makeInvoiceCorr ()
{
  fraStrList += "<tr comment=\"corr\"><td>";
  fraStrList += "<table width=\"100%\" border=\"0\">";
  fraStrList += "<tr>";
  fraStrList += "<td witdh=\"20\">&nbsp;</td>";
  fraStrList += "<td width=\"48%\"> ";
  fraStrList += "<h5>Nr faktury VAT: " + frNr->text();
  fraStrList += "<br>Data wystawienia: " + sellingDate->date().toString ("yyyy-MM-dd"); //
  fraStrList += "<br>Data sprzeda�y:" + sellingDate->date().toString ("yyyy-MM-dd");
  fraStrList += "<br>Forma p�atno�ci faktury: " + paymFry->text() + "</h5>";
  fraStrList += "</td>";
  fraStrList += "<td witdh=\"20\">&nbsp;</td>";
  fraStrList += "<td width=\"48%\" valign=\"bottom\">";
  fraStrList += "<h5>Pow�d korekty: " + reasonCombo->currentText() + "<br>";
  fraStrList += "Termin rozliczenia: " + liabDate->date().toString ("yyyy-MM-dd") + "<br>";
  fraStrList += "</h5>";
  fraStrList += "</td>";
  fraStrList += "</tr>";
  fraStrList += "</table>";
  fraStrList += "</td></tr>";
}


void Korekta::makeInvoiceGoods2 ()
{
  fraStrList += "<tr comment=\"goods\" align=\"center\"><td>";
  fraStrList += "<h4>Pozycje na fakturze przed korekt�: </h4>";
  fraStrList += "<table width=\"100%\" border=\"1\" class=\"goods\">";
  fraStrList += "<tr class=\"towaryN\">";

//  Settings sett();
// if ( sett().value/*bool*/("elinux/faktury_pozycje/Lp") .toBool())
  fraStrList += "<td width=\"20\"  align=\"center\">Lp.</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/Nazwa")  .toBool())
 fraStrList += "<td width=\"120\" align=\"center\">Nazwa</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/Kod")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">Kod</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/pkwiu")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">PKWiU</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/ilosc")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">Ilo��</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/jm")  .toBool())
  fraStrList += "<td width=\"20\" align=\"center\">Jm.</td>";
//if ( sett().value/*bool*/("elinux/faktury_pozycje/cenajedn")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">Cena jdn.</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/wartnetto")  .toBool())
//  fraStrList += "<td width=\"60\" align=\"center\">Warto�� Netto</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/rabatperc")  .toBool())
//  fraStrList += "<td width=\"20\" align=\"center\">Rabat %</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/rabatval")  .toBool())
//  fraStrList += "<td width=\"20\" align=\"center\">Rabat Warto��</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/nettoafter")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">Netto</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/vatval")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">Stawka VAT</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/vatprice")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">Kwota Vat</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/bruttoval")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">Warto�� Brutto</td>";
  fraStrList += "</tr>";
 int max = towaryPKor.size();
  for (int i = 0; i < max; ++i)
    {
      // double vatPrice = tableTow->text(i, 9).replace(",", ".").toDouble() - tableTow->text(i, 6).replace(",", ".").toDouble();
    QStringList rowTxt = towaryPKor[i].split("|");

      fraStrList += "<tr class=\"towaryList\">";
// lp, nazwa, kod, ilosc, jm, cena jm., netto, vat, brutto
// if ( sett().value/*bool*/("elinux/faktury_pozycje/Lp")   .toBool())
      fraStrList += "<td>&nbsp;" + rowTxt[0] + "</td>";

// if ( sett().value/*bool*/("elinux/faktury_pozycje/Nazwa")  .toBool())
 fraStrList += "<td>&nbsp;" + rowTxt[1]+ "</td>";
//if ( sett().value/*bool*/("elinux/faktury_pozycje/Kod")  .toBool())
      fraStrList += "<td>&nbsp;" + rowTxt[2] + "</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/pkwiu")  .toBool())
      fraStrList += "<td>&nbsp;" + rowTxt[3] + "</td>";
// pkwiu
// if ( sett().value/*bool*/("elinux/faktury_pozycje/ilosc")  .toBool())
      fraStrList += "<td>&nbsp;" + rowTxt[4] + "</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/jm")  .toBool())
      fraStrList += "<td>&nbsp;" + rowTxt[5] + "</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/cenajedn")  .toBool())
      fraStrList += "<td>&nbsp;" + rowTxt[6] + "</td>";
/*
      double cenajdn = tableTow->text (i, 6).toDouble ();
      double kwota =
	cenajdn * tableTow->text (i, 4).toInt ();
if ( sett().value("elinux/faktury_pozycje/wartnetto")  .toBool())
      fraStrList += "<td>&nbsp;" + fixStr (sett().numberToString(kwota)) + "</td>";	// netto
if ( sett().value("elinux/faktury_pozycje/rabatperc")  .toBool())
      fraStrList += "<td>&nbsp;" + sett().numberToString(rabatValue->value ()) + "% </td>";	// rabat
if ( sett().value("elinux/faktury_pozycje/rabatval")  .toBool())
      fraStrList += "<td>&nbsp;" + fixStr (sett().numberToString(kwota - tableTow->text (i, 7).toDouble ())) + " </td>";	// rabat value
      */
// if ( sett().value/*bool*/("elinux/faktury_pozycje/nettoafter")  .toBool())
      fraStrList += "<td>&nbsp;" + rowTxt[7] + "</td>";	// netto po rab
// if ( sett().value/*bool*/("elinux/faktury_pozycje/vatval")  .toBool())
      fraStrList += "<td>&nbsp;" + rowTxt[8] + "%</td>";
      /*
      double vatPrice = tableTow->text (i, 9).replace (",",
						       ".").toDouble () -
	tableTow->text (i,
			7).toDouble ();
			*/
      fraStrList += "<td>&nbsp;" + rowTxt[9] + "</td>";
      /*
      if ( sett().value("elinux/faktury_pozycje/vatprice")  .toBool())
      fraStrList +=
	"<td>&nbsp;" + fixStr (sett().numberToString(vatPrice)) + "</td>"; */
// if ( sett().value("elinux/faktury_pozycje/bruttoval")  .toBool())
      fraStrList += "<td>&nbsp;" + rowTxt[10] + "</td>";
      fraStrList += "</tr>";
    }

  fraStrList += "</table>";
  fraStrList += "</td></tr>";
}

void Korekta::makeInvoiceSumm2 ()
{
//   double vatPrice = sbrutto.replace (",",  ".").toDouble () -  snetto.toDouble ();
  fraStrList += "<tr comment=\"razem\" align=\"center\"><td>";
  fraStrList += "<table width=\"100%\" border=\"0\">";
  fraStrList += "<tr class=\"stawki\">";

//  Settings sett();
// if ( sett().value/*bool*/("elinux/faktury_pozycje/Lp") .toBool())
  fraStrList += "<td width=\"20\"  align=\"center\">&nbsp;</td>";
//if ( sett().value/*bool*/("elinux/faktury_pozycje/Nazwa")  .toBool())
  fraStrList += "<td width=\"120\" align=\"center\">&nbsp;</td>";
//if ( sett().value/*bool*/("elinux/faktury_pozycje/Kod")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">&nbsp;</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/pkwiu")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">&nbsp;</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/ilosc")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">&nbsp;</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/jm")  .toBool())
  fraStrList += "<td width=\"20\" align=\"center\">&nbsp;</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/cenajedn")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">&nbsp;</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/wartnetto")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">&nbsp;</td>";
/*
if ( sett().value("elinux/faktury_pozycje/rabatperc")  .toBool())
  fraStrList += "<td width=\"20\" align=\"center\">&nbsp;</td>";
if ( sett().value("elinux/faktury_pozycje/rabatval")  .toBool())
  fraStrList += "<td width=\"20\" align=\"center\">&nbsp;</td>";
*/
/*
if ( sett().value("elinux/faktury_pozycje/nettoafter")  .toBool())
if ( sett().value("elinux/faktury_pozycje/vatval")  .toBool())
if ( sett().value("elinux/faktury_pozycje/vatprice")  .toBool())
if ( sett().value("elinux/faktury_pozycje/bruttoval")  .toBool())
*/

  fraStrList += "<td width=\"140\">&nbsp;Razem:</td>";
  fraStrList += "<td width=\"60\">&nbsp;" + sett().numberToString(oldNetto, 'f', 2) + "</td>";	// netto
  fraStrList += "<td width=\"60\">&nbsp;</td>";
  fraStrList += "<td width=\"60\">&nbsp;" + sett().numberToString(oldVat, 'f', 2) + "</td>";// vat
  fraStrList += "<td width=\"60\">&nbsp;" + sett().numberToString(oldBrutto, 'f', 2) + "</td>";	// brutto



  fraStrList += "</tr>";
  fraStrList += "</table>";
  fraStrList += "</td></tr>";
}


void Korekta::makeInvoiceGoods ()
{
  fraStrList += "<tr comment=\"goods\" align=\"center\"><td>";
  fraStrList += "<h4>Pozycje na fakturze po korekcie: </h4>";
  fraStrList += "<table width=\"100%\" border=\"1\" class=\"goods\">";
  fraStrList += "<tr class=\"towaryN\">";

//  Settings sett();
// if ( sett().value/*bool*/("elinux/faktury_pozycje/Lp") .toBool())
  fraStrList += "<td width=\"20\"  align=\"center\">Lp.</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/Nazwa")  .toBool())
 fraStrList += "<td width=\"120\" align=\"center\">Nazwa</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/Kod")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">Kod</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/pkwiu")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">PKWiU</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/ilosc")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">Ilo��</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/jm")  .toBool())
  fraStrList += "<td width=\"20\" align=\"center\">Jm.</td>";
//if ( sett().value/*bool*/("elinux/faktury_pozycje/cenajedn")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">Cena jdn.</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/wartnetto")  .toBool())
//  fraStrList += "<td width=\"60\" align=\"center\">Warto�� Netto</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/rabatperc")  .toBool())
//  fraStrList += "<td width=\"20\" align=\"center\">Rabat %</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/rabatval")  .toBool())
//  fraStrList += "<td width=\"20\" align=\"center\">Rabat Warto��</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/nettoafter")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">Netto</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/vatval")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">Stawka VAT</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/vatprice")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">Kwota Vat</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/bruttoval")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">Warto�� Brutto</td>";
  fraStrList += "</tr>";

  for (int i = 0; i < tableTow->rowCount (); ++i)
    {
      // double vatPrice = tableTow->text(i, 9).replace(",", ".").toDouble() - tableTow->text(i, 6).replace(",", ".").toDouble();
      fraStrList += "<tr class=\"towaryList\">";
// lp, nazwa, kod, ilosc, jm, cena jm., netto, vat, brutto
// if ( sett().value/*bool*/("elinux/faktury_pozycje/Lp")   .toBool())
      fraStrList += "<td>&nbsp;" + tableTow->item (i, 0)->text() + "</td>";

// if ( sett().value/*bool*/("elinux/faktury_pozycje/Nazwa")  .toBool())
 fraStrList += "<td>&nbsp;" + tableTow->item (i, 1)->text() + "</td>";
//if ( sett().value/*bool*/("elinux/faktury_pozycje/Kod")  .toBool())
      fraStrList += "<td>&nbsp;" + tableTow->item (i, 2)->text() + "</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/pkwiu")  .toBool())
      fraStrList += "<td>&nbsp;" + tableTow->item (i, 3)->text() + "</td>";
// pkwiu
// if ( sett().value/*bool*/("elinux/faktury_pozycje/ilosc")  .toBool())
      fraStrList += "<td>&nbsp;" + tableTow->item (i, 4)->text() + "</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/jm")  .toBool())
      fraStrList += "<td>&nbsp;" + tableTow->item (i, 5)->text() + "</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/cenajedn")  .toBool())
      fraStrList += "<td>&nbsp;" + tableTow->item (i, 6)->text() + "</td>";
/*
      double cenajdn = tableTow->text (i, 6).toDouble ();
      double kwota =
	cenajdn * tableTow->text (i, 4).toInt ();
if ( sett().value("elinux/faktury_pozycje/wartnetto")  .toBool())
      fraStrList += "<td>&nbsp;" + fixStr (sett().numberToString(kwota)) + "</td>";	// netto
if ( sett().value("elinux/faktury_pozycje/rabatperc")  .toBool())
      fraStrList += "<td>&nbsp;" + sett().numberToString(rabatValue->value ()) + "% </td>";	// rabat
if ( sett().value("elinux/faktury_pozycje/rabatval")  .toBool())
      fraStrList += "<td>&nbsp;" + fixStr (sett().numberToString(kwota - tableTow->text (i, 7).toDouble ())) + " </td>";	// rabat value
      */
// if ( sett().value("elinux/faktury_pozycje/nettoafter")  .toBool())
      fraStrList += "<td>&nbsp;" + tableTow->item(i, 7)->text() + "</td>";	// netto po rab
// if ( sett().value("elinux/faktury_pozycje/vatval")  .toBool())
      fraStrList += "<td>&nbsp;" + tableTow->item (i, 8)->text() + "%</td>";
      double vatPrice = tableTow->item(i, 9)->text().replace (",",
						       ".").toDouble () -
	tableTow->item(i,
			7)->text().toDouble ();
      fraStrList += "<td>&nbsp;" + sett().numberToString(vatPrice, 'f', 2) + "</td>";
      /*
      if ( sett().value("elinux/faktury_pozycje/vatprice")  .toBool())
      fraStrList +=
	"<td>&nbsp;" + fixStr (sett().numberToString(vatPrice)) + "</td>"; */
// if ( sett().value/*bool*/("elinux/faktury_pozycje/bruttoval")  .toBool())
      fraStrList += "<td>&nbsp;" + tableTow->item(i, 9)->text() + "</td>";
      fraStrList += "</tr>";
    }

  fraStrList += "</table>";
  fraStrList += "</td></tr>";
}

void Korekta::makeInvoiceSumm ()
{
  double vatPrice = sbrutto.replace (",",  ".").toDouble () -
    snetto.toDouble ();
  fraStrList += "<tr comment=\"razem\" align=\"center\"><td>";
  fraStrList += "<table width=\"100%\" border=\"0\">";
  fraStrList += "<tr class=\"stawki\">";

//  Settings sett();
// if ( sett().value/*bool*/("elinux/faktury_pozycje/Lp") .toBool())
  fraStrList += "<td width=\"20\"  align=\"center\">&nbsp;</td>";
//if ( sett().value/*bool*/("elinux/faktury_pozycje/Nazwa")  .toBool())
  fraStrList += "<td width=\"120\" align=\"center\">&nbsp;</td>";
//if ( sett().value/*bool*/("elinux/faktury_pozycje/Kod")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">&nbsp;</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/pkwiu")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">&nbsp;</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/ilosc")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">&nbsp;</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/jm")  .toBool())
  fraStrList += "<td width=\"20\" align=\"center\">&nbsp;</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/cenajedn")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">&nbsp;</td>";
// if ( sett().value/*bool*/("elinux/faktury_pozycje/wartnetto")  .toBool())
  fraStrList += "<td width=\"60\" align=\"center\">&nbsp;</td>";
/*
if ( sett().value("elinux/faktury_pozycje/rabatperc")  .toBool())
  fraStrList += "<td width=\"20\" align=\"center\">&nbsp;</td>";
if ( sett().value("elinux/faktury_pozycje/rabatval")  .toBool())
  fraStrList += "<td width=\"20\" align=\"center\">&nbsp;</td>";
*/
/*
if ( sett().value("elinux/faktury_pozycje/nettoafter")  .toBool())
if ( sett().value("elinux/faktury_pozycje/vatval")  .toBool())
if ( sett().value("elinux/faktury_pozycje/vatprice")  .toBool())
if ( sett().value("elinux/faktury_pozycje/bruttoval")  .toBool())
*/

  fraStrList += "<td width=\"140\">&nbsp;Razem:</td>";
  fraStrList += "<td width=\"60\">&nbsp;" + snetto + "</td>";	// netto
  fraStrList += "<td width=\"60\">&nbsp;</td>";
  fraStrList += "<td width=\"60\">&nbsp;" + sett().numberToString(vatPrice, 'f', 2) + "</td>";// vat
  fraStrList += "<td width=\"60\">&nbsp;" + sbrutto + "</td>";	// brutto



  fraStrList += "</tr>";
  fraStrList += "</table>";
  fraStrList += "</td></tr>";
}

void Korekta::makeInvoiceSummAll ()
{
  fraStrList += "<tr comment=\"podsumowanie\"><td>";
  fraStrList += "<table width=\"100%\" border=\"0\">";
  fraStrList += "<tr>";
  fraStrList += "<td witdh=\"20\">&nbsp;</td>";
  fraStrList += "<td width=\"48%\"> ";
  double endVal = diffLabel->text().replace(",", ".").toDouble();

  if ( endVal > 1)
  fraStrList += "<h4>Do zap�aty: " + diffLabel->text() + " " + currency + "</h4>";
  else
      fraStrList += "<h4>Do zwrotu: " + sett().numberToString(endVal * -1, 'f', 2) + " " + currency + "</h4>";

  fraStrList += "<h5>s�ownie:" + slownie (diffLabel->text(), currency) + "</h5>";
  fraStrList += "<h5>forma p�atno�ci: " + platCombo->currentText () + "<br>";
  fraStrList +=
    "termin p�atno�ci: " + liabDate->date().toString ("yyyy-MM-dd") + "<br>";

  QString paym1 = sett().value("elinux/faktury/paym1").toString();

  if ( paym1.left(3) == platCombo->currentText().left(3) )
  fraStrList += "<b>Zap�acono got�wk�</b> <br>";

  fraStrList += "</h5>";
  fraStrList += "<h4>" + additEdit->text () + "</h4>";

  fraStrList += "</td>";
  fraStrList += "<td witdh=\"20\">&nbsp;</td>";
  fraStrList += "<td width=\"48%\" valign=\"top\">";
  fraStrList += "<table width=\"80%\" class=\"stawki\" border=\"0\">";
  fraStrList += "<tr>";
  fraStrList +=
    "<td colspan=\"4\"><hr width=\"100%\" noshade=\"noshade\" color=\"black\" /></td>";
  fraStrList += "</tr>";
  fraStrList += "<tr>";
  fraStrList += "<td colspan=\"4\"><h5>Og�em stawkami:&nbsp;</h5></td>";	// Og�em stawkami:
  fraStrList += "</tr>";
  fraStrList += getStawkami();

  fraStrList += "<tr>";
  fraStrList += "<td>&nbsp;</td>"; // netto
  fraStrList += "<td>&nbsp;</td>"; // stawka
  fraStrList += "<td>&nbsp;</td>"; // podatek
  fraStrList += "<td>&nbsp;</td>"; // brutto
  fraStrList += "</tr>";

  fraStrList += "</table>";
  fraStrList += "</td>";
  fraStrList += "</tr>";
  fraStrList += "</table>";
  fraStrList += "</td></tr>";
}


QString Korekta::getStawkami()
{
    QStringList out;
    QStringList stawki =
      sett().value("stawki").toString().split("|");
    QMap<int, double> stawkiNetto;
    QMap<int, double> stawkiVat;
    QMap<int, double> stawkiBrutto;
    // every currency in int value is equeal vat currncy

    int ssize = stawki.size();
    // qDebug( "%d", ssize );

      for (int i = 0; i < tableTow->rowCount (); ++i)
    {
	for ( int y = 0; y < ssize; ++y )
	  {
	    if ( stawki[y] == tableTow->item(i, 8)->text() ) {
		stawkiNetto[y] += tableTow->item(i, 7)->text().replace(",", ".").toDouble();
		stawkiBrutto[y] += tableTow->item(i, 9)->text().replace(",", ".").toDouble();
		stawkiVat[y] += stawkiBrutto[y] - stawkiNetto[y];
	    } else {
		    stawkiNetto[y] += 0;
		stawkiBrutto[y] += 0;
				   		stawkiVat[y] += 0;

		}
	}
      }

	for ( int y = 0; y < ssize; ++y )
	  {
  out += "<tr>";
  out += "<td>" + sett().numberToString( stawkiNetto[y], 'f', 2) + "</td>"; // netto
  out += "<td>" + stawki[y] + "</td>"; // stawka
  out += "<td>" + sett().numberToString( stawkiVat[y] , 'f', 2) + "</td>"; // podatek
  out += "<td>" + sett().numberToString( stawkiBrutto[y] , 'f', 2) + "</td>"; // brutto
  out += "</tr>";

	}
	return out.join(" ");

}


void Korekta::makeInvoiceFooter ()
{
  fraStrList += "<tr comment=\"podpis\" align=\"center\"><td>";
  fraStrList += "<br><br><br><br>";
  fraStrList += "<table width=\"80%\" border=\"0\">";
  fraStrList += "<tr>";
  fraStrList += "<td witdh=\"20\">&nbsp;</td>";
  fraStrList += "<td witdh=\"48%\" align=\"center\"> ";
  fraStrList += "<hr width=\"100%\" noshade=\"noshade\" color=\"black\" />";
  fraStrList += "</td>";
  fraStrList += "<td witdh=\"60\">&nbsp;</td>";
  fraStrList += "<td witdh=\"20\">&nbsp;</td>";
  fraStrList += "<td witdh=\"48%\" align=\"center\"> ";
  fraStrList += "<hr width=\"100%\" noshade=\"noshade\" color=\"black\" />";
  fraStrList += "</td>";
  fraStrList += "</tr>";
  fraStrList += "<tr class=\"podpisy\">";
  fraStrList += "<td witdh=\"20\">&nbsp;</td>";
  fraStrList += "<td witdh=\"48%\" align=\"center\"> ";
  fraStrList +=
    "Imi� i nazwisko osoby upowa�nionej do wystawienia faktury VAT";
  fraStrList += "</td>";
  fraStrList += "<td witdh=\"60\">&nbsp;</td>";
  fraStrList += "<td witdh=\"20\">&nbsp;</td>";
  fraStrList += "<td witdh=\"48%\" align=\"center\"> ";
  fraStrList += "Imi� i nazwisko osoby upowa�nionej do odbioru faktury VAT";
  fraStrList += "</td>";
  fraStrList += "</tr>";
  fraStrList += "</table>";
  fraStrList += "</td></tr>";
  fraStrList += "</table>";
  fraStrList += "<tr comment=\"comment\" align=\"left\"><td>";
  fraStrList += "</td></tr>";
  fraStrList += "</table>";
  fraStrList += "</body>";
  fraStrList += "</html>";
}

void Korekta::makeInvoice ()
{
  if (kontrName->text () == "")
    {
      QMessageBox::information (this, "QFaktury", "Wybierz kontrahenta",
				QMessageBox::Ok);
      return;
    }

  if (tableTow->rowCount () == 0)
    {
      QMessageBox::information (this, "QFaktury", "Nie ma towar�w",
				QMessageBox::Ok);
      return;
    }

  fraStrList.clear();

  makeInvoiceHeadar ();
  makeInvoiceBody ();
  makeInvoiceCorr ();
  makeInvoiceGoods2 ();
  makeInvoiceSumm2 ();
  makeInvoiceGoods ();
  makeInvoiceSumm ();
  makeInvoiceSummAll ();
  makeInvoiceFooter ();

  //X print invoice
  QFile file ("/tmp/invoice.html");
  if (file.open (QIODevice::WriteOnly))
    {
      QTextStream stream (&file);
      for (QStringList::Iterator it = fraStrList.begin ();
	   it != fraStrList.end (); ++it)
	stream << *it << "\n";
      file.close ();
    }

}


void Korekta::saveInvoice ()
{
  if (kontrName->text () == "")
    {
      QMessageBox::information (this, "QFaktury", "Wybierz kontrahenta",
				QMessageBox::Ok);
      return;
    }

  if (tableTow->rowCount () == 0)
    {
      QMessageBox::information (this, "QFaktury", "Nie ma towar�w",
				QMessageBox::Ok);
      return;
    }

  QDomDocument doc ("faktury");
  QDomElement root;
  QString fileName = fName;

  QFile file;
  if (fileName == "")
    {
      fileName = QDate::currentDate ().toString ("dd-MM-yyyy");
      qDebug ()<<fileName;

      int pNumber = 0;
      file.setFileName (progDir2 + "/faktury/h" + fileName + "_" +
		    sett().numberToString(pNumber) + ".xml");
      pNumber += 1;
      ret =
	"h" + fileName + "_" + sett().numberToString(pNumber) + ".xml" + "|";

      while (file.exists ())
	{
	  file.setFileName (progDir2 + "/faktury/h" + fileName + "_" +
			sett().numberToString(pNumber) + ".xml");
	  ret =
	    "h" + fileName + "_" + sett().numberToString(pNumber) + ".xml" + "|";
	  pNumber += 1;
	}
      // qDebug( "2" );
    }
  else
    {
      file.setFileName (progDir2 + "/faktury/" + fileName);
      ret = fileName + "|";
      // qDebug( "1" );
    }



  // qDebug( "adsad %s", file.name() );
  // if (!file.open (QIODevice::ReadOnly)) {

  root = doc.createElement ("korekta");
  root.setAttribute ("nr", korNr->text ());
  ret += korNr->text () + "|";
  root.setAttribute ("data.wyst",
		     QDate::currentDate ().toString ("yyyy-MM-dd"));
  root.setAttribute ("data.korekty",
		     sellingDate2->date ().toString ("yyyy-MM-dd"));
  ret += QDate::currentDate ().toString ("yyyy-MM-dd") + "|";
  root.setAttribute ("data.sprzed",
		     sellingDate->date ().toString ("yyyy-MM-dd"));


  /*
  if (windowTitle ().right (3) == "VAT")
    {
      root.setAttribute ("type", "FVAT");
      sett()1.writeEntry ("faktury/fvat", frNr->text ());
      ret += "FVAT|";
    }
  else
    {
      root.setAttribute ("type", "FPro");
      sett()1.writeEntry ("faktury/fpro", frNr->text ());
      ret += "FPro|";
    }
  */
   ret += "korekta|";
  root.setAttribute ("type", "korekta");
 sett().setValue("korNr", korNr->text ());

  sett().endGroup ();

  doc.appendChild (root);

  QDomElement sprzedawca;
  sprzedawca = doc.createElement ("sprzedawca");
  QSettings userSettings;
  sprzedawca.setAttribute ("nazwa",
		  userSettings.value ("przelewy/user/nazwa").toString());
  sprzedawca.setAttribute ("kod", userSettings.value ("przelewy/user/kod").toString());
  sprzedawca.setAttribute ("miasto",
		  userSettings.value ("przelewy/user/miejscowosc").toString());
  sprzedawca.setAttribute ("ulica",
		  userSettings.value ("przelewy/user/adres").toString());
  sprzedawca.setAttribute ("nip", userSettings.value ("przelewy/user/nip").toString());
  sprzedawca.setAttribute ("konto",
		  userSettings.value ("przelewy/user/konto").toString().
			   replace (" ", "-"));
  root.appendChild (sprzedawca);

  QDomElement nabywca;
  nabywca = doc.createElement ("nabywca");
  QStringList kht = kontrName->text ().split(",");
  /* here can be bug, if kontrahent name would be with commas
     so what, block this
   */
  nabywca.setAttribute ("nazwa", kht[0]);
  ret += kht[0] + "|";
  nabywca.setAttribute ("miasto", kht[1]);
  nabywca.setAttribute ("ulica", kht[2]);

  nabywca.setAttribute ("nip", kht[3].replace (" ", "").replace ("NIP:", ""));
  ret += kht[3].replace (" ", "").replace ("NIP:", "");
  root.appendChild (nabywca);

// sellingDate2

  QDomElement towary;
  QDomElement goods;
  goods = doc.createElement ("goods");
  goods.setAttribute ("goods::rabat", sett().numberToString(rabatValue->value ()));

  for (int i = 0; i < tableTow->rowCount (); ++i)
    {
      towary = doc.createElement ("towar:" + tableTow->item (i, 0)->text());
      goods.setAttribute ("goods::count", sett().numberToString(i + 1));
      towary.setAttribute ("Lp.", tableTow->item (i, 0)->text());
      towary.setAttribute ("Nazwa", tableTow->item (i, 1)->text());
      towary.setAttribute ("Kod", tableTow->item (i, 2)->text());
      towary.setAttribute ("PKWiU", tableTow->item (i, 3)->text());
      towary.setAttribute ("Ilosc", tableTow->item (i, 4)->text());
      towary.setAttribute ("Jm.", tableTow->item (i, 5)->text());
      towary.setAttribute ("Cena_jdn.", tableTow->item (i, 6)->text());
      double cenajdn = tableTow->item (i, 6)->text().toDouble ();
      double kwota =
	cenajdn * tableTow->item (i, 4)->text().toInt ();

      towary.setAttribute ("Wartosc_Netto", sett().numberToString(kwota, 'f', 2));	// netto

      towary.setAttribute ("Rabat", sett().numberToString(rabatValue->value ()));	// rabat
      towary.setAttribute ("Netto_po_rabacie", tableTow->item (i, 7)->text());
      towary.setAttribute ("Stawka_VAT", tableTow->item (i, 8)->text());
      double vatPrice = tableTow->item (i, 9)->text().toDouble () -
      tableTow->item (i, 7)->text().toDouble ();

      towary.setAttribute ("Kwota_Vat", sett().numberToString(vatPrice, 'f', 2));
      towary.setAttribute ("Wartosc_Brutto", tableTow->item (i, 9)->text());
      goods.appendChild (towary);

    }
  root.appendChild (goods);

  QDomElement goodsRoot;
  QDomElement goodsOld;
  goodsRoot = doc.createElement ("goodsOldRoot");

  int y = towaryPKor.count();
  QStringList tmp;
  for (int i = 0; i < y; ++i)
    {
      tmp = towaryPKor[i].split("|");
      goodsOld = doc.createElement ("goodOld:" + tmp[0]);
      goodsRoot.setAttribute ("goods::count", sett().numberToString(i + 1));
      goodsOld.setAttribute ("Lp.", sett().numberToString(i + 1) );
      goodsOld.setAttribute ("Nazwa", tmp[1] );
      goodsOld.setAttribute ("Kod", tmp[2] );
      goodsOld.setAttribute ("PKWiU", tmp[3] );
      goodsOld.setAttribute ("Ilosc", tmp[4] );
      goodsOld.setAttribute ("Cena_jdn.", tmp[6] );

      goodsOld.setAttribute ("Jm.", tmp[5] );
      goodsOld.setAttribute ("Wartosc_Netto", tmp[7] );
      // goodsOld.setAttribute ("Rabat", tmp[7] );
      goodsOld.setAttribute ("Stawka_VAT", tmp[8] );
      goodsOld.setAttribute ("Kwota_Vat", tmp[9] );
      goodsOld.setAttribute ("Wartosc_Brutto", tmp[10] );
      goodsRoot.appendChild (goodsOld);
    }


  root.appendChild (goodsRoot);
  // }


  QDomElement addinfo;
  addinfo = doc.createElement ("addinfo");
  addinfo.setAttribute ("text", additEdit->text ());
  addinfo.setAttribute ("forma.plat",
			sett().numberToString(platCombo->currentIndex ()));
  addinfo.setAttribute ("przyczyna",
			sett().numberToString(reasonCombo->currentIndex ()));
  addinfo.setAttribute ("liabDate", sellingDate->date ().toString (sett().getDateFormat()));

  addinfo.setAttribute ("waluta",
			sett().numberToString(citem));
//			sett().numberToString(currCombo->currentItem ()));
  root.appendChild (addinfo);

  QString xml = doc.toString ();
  file.close ();
  file.open (QIODevice::WriteOnly);
  QTextStream ts (&file);
  ts << xml;
  // qDebug ( "ret" + ret );
  file.close ();
  accept ();
}

QString Korekta::numbersCount(int in, int x)
{
    QString tmp2, tmp = sett().numberToString(in);
    tmp2 = "";
    int incr = x - tmp.length();
    for (int i = 0; i < incr; ++i)
	tmp2 += "0";
    return tmp2 + tmp;
}

void Korekta::backBtnClick()
{
  QString tmp;
  QString prefix;
//  int numbers;

   tmp = sett().value ("elinux/faktury/korNr").toString();
   prefix = sett().value ("elinux/faktury/prefix").toString();

  QStringList one1 = tmp.split("/");
  one1[0] = one1[0].remove(prefix);
  // qDebug( one1[0] );
  int nr = one1[0].toInt () + 1;
  lastInvoice = prefix + numbersCount(nr, sett().value("elinux/faktury/chars_in_symbol").toInt());

  if ( sett().value/*bool*/ ("elinux/faktury/day") .toBool())
     lastInvoice += "/" + QDate::currentDate ().toString ("dd");

  if ( sett().value/*bool*/ ("elinux/faktury/month") .toBool())
     lastInvoice += "/" + QDate::currentDate ().toString ("MM");

  if ( sett().value/*bool*/ ("elinux/faktury/year") .toBool()) {
  if ( !sett().value/*bool*/ ("elinux/faktury/shortYear") .toBool())
     lastInvoice += "/" + QDate::currentDate ().toString ("yy");
  else
     lastInvoice += "/" + QDate::currentDate ().toString ("yyyy");
 }


  korNr->setText (lastInvoice);
}


/*
void Korekta::backBtnClick()
{
  QString tmp;
  Settings sett();

  if (windowTitle ().right (3) == "VAT")
    {
      tmp = sett().value ("elinux/faktury/fvat");
    }
  else
    {
      tmp = sett().value ("elinux/faktury/fpro");
    }
   tmp = sett().value ("elinux/faktury/korNr");

  QStringList one1 = QStringList::split ("/", tmp);
  int nr = one1[0].toInt () + 1;
  lastInvoice =
    sett().numberToString(nr) + "/" + QDate::currentDate ().toString ("MM/yyyy");
  korNr->setText (lastInvoice);
}
*/
Korekta::Korekta(QWidget *parent): QDialog(parent) {
    setupUi(this);
    init();
}
