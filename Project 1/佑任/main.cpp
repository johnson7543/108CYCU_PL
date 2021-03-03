# include <stdio.h>
# include <iostream> // CIN
# include <string> // STRING
# include <vector> // VECTOR
# include <stdlib.h> // atoi
# define TOLERANCE 0.0001

static int uTestNumber = 0 ; // �L��debug
using namespace std ;

// �U�ؼ���
// 0:INPUTEOF     1:ASSIGN:=    2:SIGN+-   3:NUM    4:IDENT
// 5:EQ=          6:SEMI;       7:ADD+     8:SUB-   9:MULT*
// 10:DIV/        11:BEQ���L=   12:LG<>    13:G>    14:L<
// 15:GE>=        16:LE<=       17:RP)     18:LP(   19:ADDORSIGN
//  20:SUBORSIGN  21:POSIVITE�� 22:NEGATIVE�t 23:QUIT 24:FLNUM�p��
typedef enum
{
  INPUTEOF, ASSIGN, SIGN, NUM, IDENT, EQ, SEMI,
  ADD, SUB, MULT, DIV, BEQ, LG, G, L, GE, LE, RP, LP,
  ADDORSIGN, SUBORSIGN, POSIVITE, NEGATIVE, QUIT, FLNUM
}
TOKENTYPE ;

void ThrowEND( TOKENTYPE type ) {
  throw type ; // ��X���_�T��
} // ThrowEND()

void ThrowERROR() {
  string errorMSG = "Error" ;
  throw errorMSG ; // ��X���~���T��
} // ThrowERROR()

void ThrowERROR1( char ch ) {
  string str = "Unrecognized token with first char : '" ;
  string errorMSG = str + ch ;
  errorMSG = errorMSG + "'" ;
  throw errorMSG ; // ��X���~���T��
} // ThrowERROR1()

void ThrowERROR2( string str ) {
  string errorMSG = "Unexpected token : '" + str ;
  errorMSG = errorMSG + "'" ;
  throw errorMSG ; // �ߥX���~�ҥ~ERROR2
} // ThrowERROR2()

void ThrowERROR3( string str ) {
  string errorMSG = "Undefined identifier : '" + str ;
  errorMSG = errorMSG + "'" ; // THROW�X�h���T��
  throw errorMSG ; // �ߥX���~�ҥ~ERROR2
} // ThrowERROR3()

class CharDetail { // �ΨӦs��@��CHAR�����c
  public:
  char mCh  ; // �r��
  int mLineNumber ; // ��
  int mCloumnNumber ; // �C
  CharDetail() { // �غc�l
    mCh = '\0' ;
    mLineNumber = 0 ;
    mCloumnNumber = 0 ;
  } // CharDetail()
}; // class CharDetail

class TokenDetail { // �ΨӦs��@��TOKEN�����c
  public:
  TOKENTYPE mTokenType ; // �O�ƻ����O��
  string mToken ; // Token�O�ƻ�
  int mTokenLine ; // ��token�O���@�檺
  TokenDetail() { // �غc�l
    mTokenLine = 0 ;
  } // TokenDetail()
}; // class Token

class IDDetail { // �ΨӦs��@��ID�����c
  public:
  string mIDName ; // �W�l
  int mIDTokenLine ; // ��
  float mIDValue ; // ID�ҥN����
  bool mIDisfloat ;  // �p��or���
  IDDetail() { // �غc�l
    mIDTokenLine = 0 ;
    mIDValue = 0 ;
  } // IDDetail()
}; // class IDDetail

class Scanner { // Class Scanner
  private:

  char mCh ; // �r��
  int mLineNumber ; // �r������
  int mCloumnNumber ; // �r�����C

  vector< CharDetail > mCharVector ;// ��@��Char�M��LineNumber�MCloumnNumber�f�f�f�f�f�f�f�f�f
  vector< TokenDetail > mPeekToken ; // �Ȧs�@�Ӱ��ݪ�token

  public:
  Scanner() { // Scanner�غc�l
    mLineNumber = 0 ;
    mCloumnNumber = 0 ;
  } // Scanner()

  void ClearToEnter() { // �N��JŪ�촫�欰��
    while ( mCh != '\n' ) GetTheNextChar() ; // ��GetTheNextChar()����Ū�촫�欰��
  } // ClearToEnter()

  bool IsDigit( char ch ) { // �O�_���Ʀr
    if ( ch >= 48 && ch <= 57 ) return true ;
    else return false ;
  } // IsDigit()

  bool IsLetter( char ch ) { // �O�_���j�p�g���r��
    if ( ( ch >= 65 && ch <= 90 ) || ( ch >= 97 && ch <= 122 ) ) return true ;
    else return false ;
  } // IsLetter()

  bool IsDelimiter( char ch ) { // �O�_���Ÿ�
    if ( ch == ':' || ch == ';' || ch == '+' || ch == '-' || ch == '*' || ch == '/' ||  ch == '=' ||
         ch == '<' || ch == '>' || ch == '(' || ch == ')' ) return true ;
    else return false ;
  } // IsDelimiter()

  void ClearmPeekToken() { // �M�����ݪ�buffer
    mPeekToken.clear() ;
  } // ClearmPeekToken()

  TokenDetail TokenIsID() { // �h��ID�ѤU������Ū���
    // cout << "In TokenIsID()" << endl ; // debug��
    TokenDetail mTempToken ; // �Ȧs
    string token ; // token
    char mPeekch ; // ���ݪ��r��
    token = token + mCh ; // ��_��
    mPeekch = cin.peek() ; // ���ݤU�@��char
    while ( IsLetter( mPeekch ) || IsDigit( mPeekch  ) || ( mPeekch == '_' ) ) { // �u�n�ŦXID�������B��_��
      GetTheNextChar() ; // Ū�J��谽�ݪ�����
      token = token + mCh ; // ��_��
      mPeekch = cin.peek() ; // ���ݤU�@��char
    } // while()

    mTempToken.mTokenType = IDENT ; // �]�w��
    mTempToken.mToken = token ; // �]�w��
    mTempToken.mTokenLine = mLineNumber ; // �]�w��
    return mTempToken ;
    // cout << "Out TokenIsID()" << endl ; // debug��
  } // TokenIsID()

  TokenDetail TokenIsNUM() { // �h��NUM�ѤU������Ū���(���i��O�p��)
    TokenDetail mTempToken ; // �Ȧs
    // cout << "In TokenIsNUM()" << endl ; // debug��
    string token ;
    token = token + mCh ; // ��_��
    char mPeekch ; // ���ݪ��r��
    bool dot = false ; // �P�_���S���p���I

    if ( mCh == '.' ) { // �I�}�Y����
      dot = true ;
      mPeekch = cin.peek() ;
      while ( IsDigit( mPeekch ) ) { // �p�G���ݪ��O�Ʀr
        GetTheNextChar() ; // Ū�J��谽�ݪ�����
        token = token + mCh ; // ��_��
        mPeekch = cin.peek() ; // �~�򰽬�
      } // while()
    } // if()
    else if ( IsDigit( mCh ) ) { // �Ʀr�}�Y���Ʀr
      mPeekch = cin.peek() ; // ���ݬ�
      while ( IsDigit( mPeekch ) ) {  // �p�G���ݪ��O�Ʀr
        GetTheNextChar() ; // Ū�J��谽�ݪ�����
        token = token + mCh ; // ��_��
        mPeekch = cin.peek() ; // �~�򰽬�
      } // while()

      if ( mPeekch == '.' ) { // �p�G���ݪ��O�@���I
        dot = true ;
        GetTheNextChar() ; // Ū�J��谽�ݪ�����
        token = token + mCh ; // ��_��
        mPeekch = cin.peek() ; // �~�򰽬�
        while ( IsDigit( mPeekch ) ) {  // �p�G�S���ݨ�O�Ʀr
          GetTheNextChar() ; // Ū�J��谽�ݪ�����
          token = token + mCh ; // ��_��
          mPeekch = cin.peek() ; // �~�򰽬�
        } // while()
      } // if()
    } // else if()

    if ( dot == true ) {
      mTempToken.mTokenType = FLNUM ; // �]�w��
      mTempToken.mToken = token ; // �]�w��
      mTempToken.mTokenLine = mLineNumber ; // �]�w��
    } // if()
    else if ( dot == false ) {
      mTempToken.mTokenType = NUM ; // �]�w��
      mTempToken.mToken = token ; // �]�w��
      mTempToken.mTokenLine = mLineNumber ; // �]�w��
    } // else if()

    return mTempToken ;
  } // TokenIsNUM()

  TokenDetail TokenIsDelimiter() { // ���P�_�B�h��Delimiter�ѤU������Ū���
    TokenDetail mTempToken ; // �Ȧs
    // cout << "In TokenIsDelimiter()" << endl ; // debug��
    string token ;
    char mPeekch ; // ���ݪ��r��

    if ( mCh == ':' ) { // �p�G�O:
      token = token + mCh ; // ��_��
      mPeekch = cin.peek() ; // ���ݤU�@��char
      if ( mPeekch == '=' ) { // �p�G�O=
        GetTheNextChar() ; // Ū�J��谽�ݪ�����
        token = token + mCh ; // ��_��
        mTempToken.mTokenType = ASSIGN ; // �]�w��
        mTempToken.mToken = token ; // �]�w��
        mTempToken.mTokenLine = mLineNumber ; // �]�w��
      } // if()
      else { // �Ĥ@�hERROR:Unrecognized token with first char
        ThrowERROR1( mCh ) ;
      } // else
    } // if()
    else if ( mCh == '<' ) { // '<'���ⱡ�p
      token = token + mCh ; // ��_��
      mPeekch = cin.peek() ; // ���ݤU�@��char
      if ( mPeekch == '>' ) {
        GetTheNextChar() ; // Ū�J��谽�ݪ�����
        token = token + mCh ; // ��_��
        mTempToken.mTokenType = LG ; // �]�w����
        mTempToken.mToken = token ; // �]�w��
        mTempToken.mTokenLine = mLineNumber ; // �]�w��
      } // if()
      else if ( mPeekch == '=' ) {
        GetTheNextChar() ; // Ū�J��谽�ݪ�����
        token = token + mCh ; // ��_��
        mTempToken.mTokenType = LE ; // �]�w����
        mTempToken.mToken = token ; // �]�w��
        mTempToken.mTokenLine = mLineNumber ; // �]�w��
      } // else if
      else {
        mTempToken.mTokenType = L ; // �]�w����
        mTempToken.mToken = token ; // �]�w��
        mTempToken.mTokenLine = mLineNumber ; // �]�w��
      } // else
    } // else if()
    else if ( mCh == '>' ) { // '>'���ⱡ�p
      token = token + mCh ; // ��_��
      mPeekch = cin.peek() ; // ���ݤU�@��char
      if ( mPeekch == '=' ) {
        GetTheNextChar() ; // Ū�J��谽�ݪ�����
        token = token + mCh ; // ��_��
        mTempToken.mTokenType = GE ; // �]�w����
        mTempToken.mToken = token ; // �]�w��
        mTempToken.mTokenLine = mLineNumber ; // �]�w��
      } // if()
      else {
        mTempToken.mTokenType = G ; // �]�w����
        mTempToken.mToken = token ; // �]�w��
        mTempToken.mTokenLine = mLineNumber ; // �]�w��
      } // else
    } // else if()
    else if ( mCh == ';' ) {
      token = token + mCh ; // ��_��
      mTempToken.mTokenType = SEMI ; // �]�w����
      mTempToken.mToken = token ; // �]�w��
      mTempToken.mTokenLine = mLineNumber ; // �]�w��
    } // else if()
    else if ( mCh == '+' ) {
      token = token + mCh ; // ��_��
      mTempToken.mTokenType = ADDORSIGN ; // �]�w����
      mTempToken.mToken = token ; // �]�w��
      mTempToken.mTokenLine = mLineNumber ; // �]�w��
    } // else if()
    else if ( mCh == '-' ) {
      token = token + mCh ; // ��_��
      mTempToken.mTokenType = SUBORSIGN ; // �]�w����
      mTempToken.mToken = token ; // �]�w��
      mTempToken.mTokenLine = mLineNumber ; // �]�w��
    } // else if()
    else if ( mCh == '*' ) {
      token = token + mCh ; // ��_��
      mTempToken.mTokenType = MULT ; // �]�w����
      mTempToken.mToken = token ; // �]�w��
      mTempToken.mTokenLine = mLineNumber ; // �]�w��
    } // else if()
    else if ( mCh == '/' ) {
      token = token + mCh ; // ��_��
      mTempToken.mTokenType = DIV ; // �]�w����
      mTempToken.mToken = token ; // �]�w��
      mTempToken.mTokenLine = mLineNumber ; // �]�w��
    } // else if()
    else if ( mCh == '=' ) {
      token = token + mCh ; // ��_��
      mTempToken.mTokenType = EQ ; // �]�w����
      mTempToken.mToken = token ; // �]�w��
      mTempToken.mTokenLine = mLineNumber ; // �]�w��
    } // else if()
    else if ( mCh == '(' ) {
      token = token + mCh ; // ��_��
      mTempToken.mTokenType = LP ; // �]�w����
      mTempToken.mToken = token ; // �]�w��
      mTempToken.mTokenLine = mLineNumber ; // �]�w��
    } // else if()
    else if ( mCh == ')' ) {
      token = token + mCh ; // ��_��
      mTempToken.mTokenType = RP ; // �]�w����
      mTempToken.mToken = token ; // �]�w��
      mTempToken.mTokenLine = mLineNumber ; // �]�w��
    } // else if()

    return mTempToken ;
    // cout << "Out TokenIsDelimiter()" << endl ; // debug��
  } // TokenIsDelimiter()

  // �I�s�o�Ӯɤ@�w�O�qmPeekToken���M��R��mPeekToke(�N��گu�o�������ݪ��F)
  TokenDetail GetToken() {
    // cout << "Into GetToken()" << endl ; // debug��
    TokenDetail mTempToken ; // �Ȧs
    mTempToken = mPeekToken.front() ; // ��
    // cout << "[" << mTempToken.mToken << "]" << endl ;
    mPeekToken.clear() ; // �R��
    return mTempToken ;
    // cout << "Out GetToken()" << endl ; // debug��
  } // GetToken()

  // ���ݤU�@��token�Ȧs�_��(�B���LERRORTOKEN)
  TokenDetail PeekToken() {
    // cout << "Into PeekToken()" << endl ; // debug��
    if ( mPeekToken.empty() == false ) { // mPeekToken�̭����F��N�^�ǪF��^�h
      TokenDetail mTempToken ; // �Ȧs
      mTempToken = mPeekToken.front() ;
      // cout << "["<< mTempToken.mToken << "]" << endl ;
      return mTempToken ;
    } // if()
    else { // �p�G�Ū��NŪ�@�Ӧs��mPeekTokens�ä��̵M�^�ǳo��token��T(�˳QŪ���F����٨S�A�Y�O�@��ERRORTOKEN�N���s���@��)
      TokenDetail mTempToken ; // �Ȧs
      mTempToken = TheRealGetToken() ; // �u���qINPUTŪ���r��
      mPeekToken.push_back( mTempToken ) ;
      // cout << "[" << mTempToken.mToken << "]" << endl ;
      // if ( mTempToken.mTokenType == INPUTEOF ) ThrowEND( QUIT ) ;
      return mTempToken ;
    } // else
    // cout << "Out PeekToken()" << endl ;
  } // PeekToken()

  // �qinputŪ�J�@��TOKEN�i��
  TokenDetail TheRealGetToken() {
    // cout << "into TheRealGetToken()" << endl ; // debug��
    TokenDetail mTempToken ; //
    GetTheNextNonWhiteSpaceChar() ; // Ū�U�@��token��first char
    if ( mCh == EOF ) { // �Yinput�S�F
      // cout << " INPUT EOF!!! " << endl ; // debug��
      ThrowEND( QUIT ) ;
      return mTempToken ;
    } // if()
    else if ( IsLetter( mCh ) ) { // �Ych�O�@��letter
      mTempToken = TokenIsID() ; // �I�sTokenIsID()�ӧ�X���TOKEN
      return mTempToken ;
    } // if()
    else if ( IsDigit( mCh ) || ( mCh == '.' ) ) { // �Ych�O�@�ӼƦror'.'
      mTempToken = TokenIsNUM() ; // �I�sTokenIsNUM()�ӧ�X���TOKEN
      if ( mTempToken.mToken == "." ) ThrowERROR1( '.' ) ;
      return mTempToken ;
    } // else if()
    else if ( IsDelimiter( mCh ) ) { // �YCHAR�O�@�ӲŸ� TokenIsDelimiter()�ӧ�X���TOKEN
      mTempToken = TokenIsDelimiter() ;
      return mTempToken ;
    } // else if()
    else {
      // cout << "throw" ;
      ThrowERROR1( mCh ) ; // �L�k���Ѫ��r��
    } // else

    return mTempToken ;
    // cout << "Unrecognized token with first char : " << "'" << mTokenValue[0] << "'" << endl ; // (���G)
    // cout << "out TheRealGetToken()" << endl ;// debug��
  } // TheRealGetToken()

  void GetTheNextChar() { // �l�ܬ����Ү��쪺char��line number�Mcolumn number
    CharDetail mTempChar ; // �Ȯɩ�@�ӱo�쪺char����T
    // cout << "into GetTheNextChar()" << endl ; // debug��
    if ( cin.get( mCh ) ) { // Ū�@��char
      mTempChar.mCh = mCh ; // char��Jtemp
      mTempChar.mLineNumber = mLineNumber ; // lineNumber��Jtemp
      mTempChar.mCloumnNumber = mCloumnNumber ; // CloumnNumber��Jtemp
      // cout << " CHAR: " << "[" << mTempChar.mCh << "]" << " lineNumber : " ; // debug��
      // cout << mTempChar.mLineNumber << " cloumnNumber : " << mTempChar.mCloumnNumber << endl ; // debug��
      mCharVector.push_back( mTempChar ) ; // �Nchar��JCharVector
      if ( mCh == '\n' ) { // �p�GŪ��@��'\n'
        mLineNumber = mLineNumber + 1 ; // ���+1
        mCloumnNumber = 1 ; // �C�q1�}�l23
      } // if()
      else { // �p�G���OŪ��@��'\n'
        mCloumnNumber = mCloumnNumber + 1 ; // �C+1
      } // else
    } // if()
    else { // �S��input�F
      mCh = EOF ; // �^���ثe��J�OEOF
    } // else
    // cout << "out GetTheNextChar() ;" << endl ;// debug��
  } // GetTheNextChar()

  void GetTheNextNonWhiteSpaceChar() { // ��function�n�t�d���Lcomment�Mnon-white-space char
    // cout << "into GetTheNextNonWhiteSpaceChar()" << endl ; // debug��
    GetTheNextChar() ; // �I�s�����@��char
    char mPeekch ; // ���ݪ��r��
    while ( mCh == ' ' || mCh == '\n' || mCh == '\t' || mCh == '/' ) { // �p�GmCh�O�o�|�Ө䤤�@��
      if ( mCh == '/' ) { // �p�GmCh�O'/'
        mPeekch = cin.peek() ; // ���ݤU�@��
        if ( mPeekch == '/' ) { // �p�G�U�@�Ӥ]�O/�N��᳣�O����
          GetTheNextChar() ;  // �����誺�ĤG�ӱ׽uŪ��
          ClearToEnter() ; // Ū��'\n' ����
        } // if() �J��ĤG�ӵ��ѱ׽u '/
        else { // �u���@��/�N��u�O���k
          return ;
        } // else()
      } // if()
      else if ( mCh == ' ' || mCh == '\n' || mCh == '\t' ) { // �p�GmCh�Onon-white-space
        GetTheNextChar() ;  // �n���L�o�Ǧr�n���UŪ
      } // else if
    } // while()
      // cout << "out GetTheNextNonWhiteSpaceChar() " << endl ; // debug��
  } // GetTheNextNonWhiteSpaceChar()

}; // Class Scanner

class Parser { // Class Parser
  private:
  Scanner mScanner ; // ��Parser��scanner

  vector < TokenDetail > mPassSyntaxVector ; // �Ȧs�@�����O(�q�L��k��)(Token�M���O�M���X �f�f�f�f�f�f�f�f�f)
  vector < IDDetail > mDefinedIDVector ; // �s��w�g�w�q�L��ID(�W�r�A�ȡA��)
  bool mIsfloat ;
  public:
  Parser( Scanner sc ) { // �غc�l (�ǤJ�bmain�ŧi��scanner)
    mScanner = sc ; // �Nscanner�����ϥΦP�@��
    mIsfloat = false ;
  } // Parser()

  void PushInTomDefinedIDVector( IDDetail mTempID ) { // �s�W�@��ID��mDefinedIDVector
    mDefinedIDVector.push_back( mTempID ) ; // �N�@�ӷs��ID���i�h
  } // PushInTomDefinedIDVector()

  void SetTheIDValue( string IDname, float IDvalue ) { // ��mDefinedIDVector�]�w�ק�H�w�q�L��ID��
    for ( int i = 0 ; i < mDefinedIDVector.size() ; i++ ) {
      if ( IDname == mDefinedIDVector[i].mIDName ) {
        mDefinedIDVector[i].mIDValue = IDvalue ; // ��ID���ȧ�s
        return ;
      } // if()
    } // for
  } // SetTheIDValue()

  bool InThemDefinedIDVectorOrNot( string IDname ) { // �P�_��ID���S���Q�w�q�L
    for ( int i = 0 ; i < mDefinedIDVector.size() ; i++ ) {
      if ( IDname == mDefinedIDVector[i].mIDName ) return true ;
    } // for()

    return false ;
  } // InThemDefinedIDVectorOrNot()

  void GetTheIDValue( string IDname, float &IDvalue ) { // �p�G���Q�w�q�A�����w�Q�w�q�L��ID���ƭ�
    for ( int i = 0 ; i < mDefinedIDVector.size() ; i++ ) {
      if ( IDname == mDefinedIDVector[i].mIDName ) {
        IDvalue = mDefinedIDVector[i].mIDValue ;
        return ;
      } // if
    } // for()
  } // GetTheIDValue()

  void SetTheIDNumType( string IDname, bool isfloat ) { // �h�]�w��ID���ƭȬO�p���٬O���..................
    for ( int i = 0 ; i < mDefinedIDVector.size() ; i++ ) {
      if ( IDname == mDefinedIDVector[i].mIDName ) {
        mDefinedIDVector[i].mIDisfloat = isfloat ;
        return ;
      } // if
    } // for()
  } // SetTheIDNumType()

  void GetTheIDNumType( string IDname, bool &isfloat ) { // ���oID���ƭȬO�p���٬O���
    for ( int i = 0 ; i < mDefinedIDVector.size() ; i++ ) {
      if ( IDname == mDefinedIDVector[i].mIDName ) {
        isfloat = mDefinedIDVector[i].mIDisfloat  ;
        // cout << isfloat << endl ;
        return ;
      } // if()
    } // for()
  } // GetTheIDNumType()
  // ------------------------------------------��k���R-------------------------
  void Factor() { // ��k<Factor>
    // <Factor> ::= IDENT | [ SIGN ] NUM | '(' <ArithExp> ')'
    TokenDetail mTempToken ; // �Ȧs
    mTempToken = mScanner.PeekToken() ; // ���ݤU�@��Token

    // �p�G�������ݩ�<Factor>�Q�n���N�����^��false
    if ( ( mTempToken.mTokenType != IDENT && mTempToken.mTokenType != ADDORSIGN &&
           mTempToken.mTokenType != SUBORSIGN && mTempToken.mTokenType != NUM &&
           mTempToken.mTokenType != LP && mTempToken.mTokenType != FLNUM ) ) {
      ThrowERROR2( mTempToken.mToken ) ;
    } // if()

    // �Y�OIDENT�Ϊ̤��O�b<command>�Ĥ@�Ӯ��쪺"quit"���]�O�@��ID
    if ( mTempToken.mTokenType == IDENT  ) {
      if ( InThemDefinedIDVectorOrNot( mTempToken.mToken ) == false ) { // �p�G�S���Q�w�q
        ThrowERROR3( mTempToken.mToken ) ; // ��X�ҥ~3
      } // if()
      // �즹����A�@�w�OID�B���w�q
      mTempToken = mScanner.GetToken() ; // �������ݪ�����ID
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      return ; // ����
    } // if()

    // �p�G�O�@��ADDORSIGN
    else if ( mTempToken.mTokenType == ADDORSIGN ) {
      mTempToken = mScanner.GetToken() ; // �������ݪ�����(+)
      mTempToken.mTokenType = POSIVITE ; // �NADDORSIGN�令POSIVITE�A�]���b�o�̤@�w�O���t
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      mTempToken = mScanner.PeekToken() ; // ���ݤU�@��Token
      if ( mTempToken.mTokenType != NUM && mTempToken.mTokenType != FLNUM ) { // �p�G�U�@�Ӥ��ONUM�]���OFLNUM
        ThrowERROR2( mTempToken.mToken ) ;
      } // if()
      // �즹����A�U�@�Ӥ@�w�ONUM
      mTempToken = mScanner.GetToken() ; // �������ݪ�����NUM
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      return ; // ����
    } // else if()

    // �p�G�OASUBORSIGN
    else if ( mTempToken.mTokenType == SUBORSIGN ) {
      mTempToken = mScanner.GetToken() ; // �������ݪ�����(-)
      mTempToken.mTokenType = NEGATIVE ; // �NSUBORSIGN�令NEGATIVE�A�]���b�o�̤@�w�OSIGN
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      mTempToken = mScanner.PeekToken() ; // ���ݤU�@��Token
      if ( mTempToken.mTokenType != NUM && mTempToken.mTokenType != FLNUM ) { // �p�G���ONUM
        ThrowERROR2( mTempToken.mToken ) ;
      } // if()
      // �Y�즹�A�U�@�Ӥ@�w�ONUM��FLNUM
      mTempToken = mScanner.GetToken() ; // �������ݪ�����NUM
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      return ; // ����
    } // else if()
    // �p�G�����ONUMorFLNUM(�i�H���@�w�n��SIGN)
    else if ( mTempToken.mTokenType == NUM ||  mTempToken.mTokenType == FLNUM ) {
      mTempToken = mScanner.GetToken() ; // �������ݪ�����
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      return ; // ����
    } // else if()
    // �Y�즹�A�U�@�Ӥ@�w�O���A��LP
    else {
      mTempToken = mScanner.GetToken() ; // �������ݪ�����LP
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      ArithExp() ; // �I�sArithExp()
      mTempToken = mScanner.PeekToken() ; // ���ݤU�@��Token
      if ( mTempToken.mTokenType != RP ) { // �p�G���O�k�A��RP
        ThrowERROR2( mTempToken.mToken ) ;
      } // if()
      // �즹����U�@�Ӥ@�w�ORP
      mTempToken = mScanner.GetToken() ; // �������ݪ�����
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      return ; // ����
    } // else
  } // Factor()

  void Term() { // ��k<Term>
    // <Term> ::= <Factor> { '*' <Factor> | '/' <Factor> }
    TokenDetail mTempToken ; // �Ȧs
    Factor() ; // �I�s�Ĥ@��Factor()
    // �즹����Ĥ@��Factor�@�w���T

    mTempToken = mScanner.PeekToken() ; // ���ݤU�@��TOKEN
    if ( mTempToken.mTokenType != MULT && mTempToken.mTokenType != DIV ) return ; // ����

    // �U�@�ӬO�����N�~��
    while ( mTempToken.mTokenType == MULT || mTempToken.mTokenType == DIV ) {
      // �즹����U�@�Ӥ@�w�O*��/
      mTempToken = mScanner.GetToken() ; // �������ݪ�����
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      Factor() ; // �I�s�ĤG��Factor()
      // �즹����Factor�ĤG���@�w���T
      mTempToken = mScanner.PeekToken() ; // ���ݤU�@��TOKEN
      if ( mTempToken.mTokenType != MULT && mTempToken.mTokenType != DIV ) return ; // ����
    } // while()
    // �즹������T
    return ; // ����

  } // Term()

  void ArithExp() { // ��k<ArithExp>
    // <ArithExp> ::= <Term> { '+' <Term> | '-' <Term> }
    TokenDetail mTempToken ; // �Ȧs
    Term() ; // �I�sTerm()
    // �즹����Ĥ@��<Term>�@�w���T
    mTempToken = mScanner.PeekToken() ; // ���ݤU�@��TOKEN
    if ( mTempToken.mTokenType != ADDORSIGN && mTempToken.mTokenType != SUBORSIGN ) return ; // ����
    // �u�n�U�@�ӬO�[��N�~��
    while ( mTempToken.mTokenType == ADDORSIGN || mTempToken.mTokenType == SUBORSIGN ) {

      // �즹����U�@�Ӥ@�w�OADDORSIGN or SUBORSIGN
      mTempToken = mScanner.GetToken() ;  // �������ݪ�����

      // �]���b�������@�w�O��ª��[��O���t
      if ( mTempToken.mTokenType == ADDORSIGN ) mTempToken.mTokenType = ADD ; // �NADDORSIGN��^ADD
      else mTempToken.mTokenType = SUB ; // �NSUBORSIGN��^SUB
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      Term() ; // �I�sTerm()
      // �즹����Term�ĤG���@�w���T
      mTempToken = mScanner.PeekToken() ; // ���ݤU�@��TOKEN
      if ( mTempToken.mTokenType != ADDORSIGN && mTempToken.mTokenType != SUBORSIGN ) return ; // ����
    } // while() // �Y�S��������~�̵M�~��

    return ;
  } // ArithExp()

  void BooleanOperator( bool &Correct ) { // ��k<BooleanOperator>
    // <BooleanOprator> ::= '=' | '<>' | '>' | '<' | '>=' | '<='
    TokenDetail mTempToken ; // �Ȧs
    mTempToken = mScanner.PeekToken() ; // ���ݤU�@��TOKEN

    // �p�G���O���N�@�ӥ��L�Ÿ�
    if ( mTempToken.mTokenType != EQ && mTempToken.mTokenType != LG
         && mTempToken.mTokenType != G && mTempToken.mTokenType != L
         && mTempToken.mTokenType != GE && mTempToken.mTokenType != LE ) {
      Correct = false ;
      return ;
    } // if()
    // �즹����U�@�Ӥ@�w�O�ӥ��L�Ÿ�

    Correct = true ; // �O�ӥ��L�L�Ÿ�
    return ; // ����
  } // BooleanOperator()

  void NOT_ID_StartFactor() { // ��k<NOT_ID_StartFactor>
    // <NOT_ID_StartFactor> ::= [ SIGN ] NUM | '(' <ArithExp> ')'
    TokenDetail mTempToken ; // �Ȧs
    mTempToken = mScanner.PeekToken() ; // ���ݤU�@��TOKEN

    // �Y���O<NOT_ID_StartFactor>�ҹw����type
    if ( mTempToken.mTokenType != ADDORSIGN && mTempToken.mTokenType != SUBORSIGN
         && mTempToken.mTokenType != NUM && mTempToken.mTokenType != LP && mTempToken.mTokenType != FLNUM ) {
      ThrowERROR2( mTempToken.mToken  ) ;
    } // if()
    // �즸����U�@�Ӥ@�w�O'ADDORSIGN' OR 'SUBORSIGN' OR 'NUM' OR '('
    if ( mTempToken.mTokenType == ADDORSIGN ) { // �Y�OADDORSIGN
      mTempToken = mScanner.GetToken() ; // �������ݪ�����
      mTempToken.mTokenType = POSIVITE ; // �NADDORSIGN�令���A�]���b���@�w�O�ӥ���
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      mTempToken = mScanner.PeekToken() ; // ���ݤU�@��TOKEN
      if ( mTempToken.mTokenType != NUM && mTempToken.mTokenType != FLNUM ) { // �Y���ONUM OR FLNUM
        ThrowERROR2( mTempToken.mToken ) ;
      } // if()
      // �즹����U�@�Ӥ@�w�ONUM
      mTempToken = mScanner.GetToken() ; // �������ݪ�����
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      return ; // ����
    } // if()

    // �p�G�O�@��SUBORSIGN
    else if ( mTempToken.mTokenType == SUBORSIGN ) {
      mTempToken = mScanner.GetToken() ; // �������ݪ�����
      mTempToken.mTokenType = NEGATIVE ; // �NSUBORSIGN�令�t�A�]���b���@�w�O�ӭt��
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      mTempToken = mScanner.PeekToken() ; // ���ݤU�@��TOKEN
      if ( mTempToken.mTokenType != NUM && mTempToken.mTokenType != FLNUM ) { // �Y���ONUM OR FLNUM
        ThrowERROR2( mTempToken.mToken ) ;
      } // if()
      // �즹����U�@�Ӥ@�w�ONUM
      mTempToken = mScanner.GetToken() ; // �������ݪ�����
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      return ; // ����
    } // else if()
    // �Y�U�@�Ӫ����O�@��NUM OR FLNUM
    else if ( mTempToken.mTokenType == NUM || mTempToken.mTokenType == FLNUM ) {
      mTempToken = mScanner.GetToken() ; // �������ݪ�����
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      return ; // ����
    } // else if()
    // �즹����U�@�Ӥ@�w�O���A��LP
    else {
      mTempToken = mScanner.GetToken() ; // �������ݪ�����LP
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      ArithExp() ; // �I�sArithExp()
      mTempToken = mScanner.PeekToken() ; // ���ݤU�@��TOKEN
      if ( mTempToken.mTokenType != RP ) { // �p�G���O�k�A��RP
        ThrowERROR2( mTempToken.mToken ) ;
      } // if()
      // �즹����U�@�Ӥ@�w�O�k�A��
      mTempToken = mScanner.GetToken() ; // �������ݪ�����
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      return ; // ����
    } // else
  } // NOT_ID_StartFactor()

  void NOT_ID_StartTerm() { // ��k<NOT_ID_StartTerm>
    // <NOT_ID_StartTerm> ::= <NOT_ID_StartFactor> { '*' <Factor> | '/' <Factor> }
    TokenDetail mTempToken ; // �Ȧs
    NOT_ID_StartFactor() ; // �I�sNOT_ID_StartFactor()
    // �즹����NOT_ID_StartFactor()���T

    mTempToken = mScanner.PeekToken() ; // ���ݤU�@��TOKEN
    if ( mTempToken.mTokenType != MULT && mTempToken.mTokenType != DIV ) return ; // ����

    while ( mTempToken.mTokenType == MULT || mTempToken.mTokenType == DIV ) {
      // �즹����U�@�Ӥ@�w�O '*' or '/'
      mTempToken = mScanner.GetToken() ; // �������ݪ�����
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      Factor() ; // �I�sFactor()
      // �즹����Factor�@�w���T
      mTempToken = mScanner.PeekToken() ; // ���ݤU�@��TOKEN
      if ( mTempToken.mTokenType != MULT && mTempToken.mTokenType != DIV ) return ; // ����
    } // while()

    return ; // ����
  } // NOT_ID_StartTerm()

  void NOT_ID_StartArithExp() { // ��k<NOT_ID_StartArithExp>
    // <NOT_ID_StartArithExp>  ::= <NOT_ID_StartTerm> { '+' <Term> | '-' <Term> }
    TokenDetail mTempToken ; // �Ȧs
    NOT_ID_StartTerm() ; // �I�sNOT_ID_StartTerm()
    // �즹����NOT_ID_StartTerm()�@�w���T
    mTempToken = mScanner.PeekToken() ; // ���ݤU�@��TOKEN
    if ( mTempToken.mTokenType != ADDORSIGN && mTempToken.mTokenType != SUBORSIGN ) return ; // ����

    while ( mTempToken.mTokenType == ADDORSIGN || mTempToken.mTokenType == SUBORSIGN ) {

      // �즹����U�@�Ӥ@�w�O'ADDORSIGN' or 'SUBORSIGN'
      mTempToken = mScanner.GetToken() ;  // �������ݪ�����
      // �b���B�@�w�O�[��Ӥ��O���t
      if ( mTempToken.mTokenType == ADDORSIGN ) mTempToken.mTokenType = ADD ; // �NADDORSIGN��^ADD
      else mTempToken.mTokenType = SUB ; // �NSUBORSIGN��^SUB

      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      Term() ; // �I�sTerm()
      // �즹����Term�@�w���T

      mTempToken = mScanner.PeekToken() ; // ���ݤU�@��TOKEN
      if ( mTempToken.mTokenType != ADDORSIGN && mTempToken.mTokenType != SUBORSIGN ) return ; // ����
    } // while()

    return ;
  } // NOT_ID_StartArithExp()

  void NOT_ID_StartArithExpOrBexp() { // ��k<NOT_ID_StartArithExpOrBexp>
    // <NOT_ID_StartArithExpOrBexp> ::= <NOT_ID_StartArithExp> [ <BooleanOperator> <ArithExp> ]
    TokenDetail mTempToken ; // �Ȧs
    NOT_ID_StartArithExp() ; // �I�sNOT_ID_StartArithExp()
    // �즹����NOT_ID_StartArithExp()�@�w���T
    // ���ۧP�_<BooleanOperator>�M<ArithExp>�n�P�ɥ��T�ΦP�ɿ��~�~�i�H!

    bool boolBooleanOperator = false ; // �ΨӧP�_<BooleanOperator>�O�_���T
    BooleanOperator( boolBooleanOperator ) ;  // �I�sBooleanOperator()
    if ( boolBooleanOperator  == true )  { // ���L���T����
      mTempToken = mScanner.GetToken() ; // ����boolean
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      ArithExp() ; // �I�sArithExp()
      return ;
    } // if()
    else {  // �p�G���L���~����(�]�����L�i���i�L)
      return ;
    } // else()���N�O�諸

  } // NOT_ID_StartArithExpOrBexp()

  void IDlessArithExpOrBexp( string idname ) { // ��k<IDlessArithExpOrBexp>
    // <IDlessArithExpOrBexp>  ::= {   '+' <Term>   | '-' <Term>
    // | '*' <Factor> | '/' <Factor>
    // }
    // [ <BooleanOperator> <ArithExp> ]

    TokenDetail mTempToken ; // �Ȧs
    mTempToken = mScanner.PeekToken() ; // ���ݤU�@��TOKEN


    while ( mTempToken.mTokenType == ADDORSIGN || mTempToken.mTokenType == SUBORSIGN
            || mTempToken.mTokenType == MULT || mTempToken.mTokenType == DIV ) {

      if ( InThemDefinedIDVectorOrNot( idname ) == false ) { // �p�G�S���Q�w�q
        ThrowERROR3( idname ) ; // ��X�ҥ~3
      } // if()

      if ( mTempToken.mTokenType == ADDORSIGN || mTempToken.mTokenType == SUBORSIGN ) {
        mTempToken = mScanner.GetToken() ; // �������ݪ�����
        // �b�������@�w�O�n�令�[��
        if ( mTempToken.mTokenType == ADDORSIGN ) mTempToken.mTokenType = ADD ; // �NADDORSIGN��^ADD
        else mTempToken.mTokenType = SUB ; // �NSUBORSIGN��^SUB

        mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
        // '+'�A'-'�N�n�P�_�᭱��<Term>
        Term() ; // �I�sTerm()
      } // if()
      else { // �p�G�O'*' or '/'
        mTempToken = mScanner.GetToken() ; // �������ݪ�����
        mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
        // '*' or '/'�N�n�P�_�᭱��<Factor>
        Factor() ; // �I�sFactor()
      } // else
      // �즹�����'+'�A'-'�A'*'�A'/'��<Term>or<Factor>�������T
      mTempToken = mScanner.PeekToken() ; // ���ݤU�@��TOKEN
    } // while()
    // �즹����W��{}���@�w���T
    // { }�P�_������ �n�P�_<BooleanOperator>�M<ArithExp>�B�n�P�ɥ��T�ΦP�ɿ��~�~�i�H!
    bool boolBooleanOperator = false ; // �ΨӧP�_<BooleanOperator>�O�_���T
    BooleanOperator( boolBooleanOperator ) ;  // �I�sBooleanOperator()
    if ( boolBooleanOperator  == true )  { // ���L���T����

      if ( InThemDefinedIDVectorOrNot( idname ) == false ) { // �p�G�S���Q�w�q
        ThrowERROR3( idname ) ; // ��X�ҥ~3
      } // if()

      mTempToken = mScanner.GetToken() ; // ����boolean
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      ArithExp() ; // �I�sArithExp()
      return ;
    } // if()
    else {  // �p�G���L���~����(�]�����L�i���i�L)
      return ;
    } // else() ���N�O�諸
  } // IDlessArithExpOrBexp()

  void Command() { // ��k<Command>
    // <Command> ::= IDENT ( ':=' <ArithExp> | <IDlessArithExpOrBexp> ) ';'
    // | <NOT_IDStartArithExpOrBexp> ';'
    // | QUIT
    TokenDetail mTempToken ; // �Ȧs
    mTempToken = mScanner.PeekToken() ; // ���ݤU�@��TOKEN
    if ( mTempToken.mTokenType == IDENT ) { // �p�G�OIDENT

      if ( mTempToken.mToken == "quit" ) ThrowEND( QUIT ) ;

      // �즹���� �@�w�O�ۥ��`��ID
      mTempToken = mScanner.GetToken() ; // �������ݪ�����ID
      string idname = mTempToken.mToken ; // ID���W�r�s�_��
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      mTempToken =  mScanner.PeekToken() ; // ���ݤU�@��TOKEN

      if ( mTempToken.mTokenType == ASSIGN ) { // �P�_�O���O':='
        mTempToken = mScanner.GetToken() ; // �������ݪ�����(:=)
        mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
        // ���ۧP�_<ArithExp>
        ArithExp() ; // �I�sArithExp()
        mTempToken = mScanner.PeekToken() ; // ���ݤU�@��token
        if ( mTempToken.mTokenType != SEMI ) { // �p�G���O';'
          ThrowERROR2( mTempToken.mToken ) ; // ��X�ҥ~2
        } // if
        // �즹����U�@�Ӥ@�w�O';'
        mTempToken = mScanner.GetToken() ; // �������ݪ�����
        mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
        return ; // ����
      } // if()
      else { // �p�GID�᭱���O':='�����p �A�ӴN�n�I�s<IDlessArithExpOrBexp>

        IDlessArithExpOrBexp( idname ) ; // �I�sIDlessArithExpOrBexp()
        // �YIDlessArithExpOrBexp���T
        mTempToken = mScanner.PeekToken() ; // ���ݤU�@��token
        if ( mTempToken.mTokenType != SEMI ) { // �p�G���O';'
          ThrowERROR2( mTempToken.mToken ) ; // ��X�ҥ~2
        } // if
        // �즹����A�U�@�Ӥ@�w�O';'
        mTempToken = mScanner.GetToken() ; // �������ݪ�����
        mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
        return ; // ����
      } // else
    } // if()
    else { // �W�z��ӱ��p�����O���� �N����<NOT_ID_StartArithExpOrBexp>
      NOT_ID_StartArithExpOrBexp() ; // �I�sNOT_ID_StartArithExpOrBexp()
      mTempToken = mScanner.PeekToken() ; // ���ݤU�@��token
      if ( mTempToken.mTokenType != SEMI ) { // �p�G���O';'
        ThrowERROR2( mTempToken.mToken ) ; // ��X�ҥ~2
      } // if
      // �즹����U�@�Ӥ@�w�O';'
      mTempToken = mScanner.GetToken() ; // �������ݪ�����
      mPassSyntaxVector.push_back( mTempToken ) ; // �ŦX��k�N��J
      return ; // ����
    } // else
  } // Command()
  // ------------------------------------------��k���R-------------------------
  // ------------------------------------------�p����O-------------------------
  // �p��O�w��mPassSyntaxVector�Ӷi��ާ@
  void CalFactor( float &value ) { // �p��<Factor>
    // <Factor> ::= IDENT | [ SIGN ] NUM | '(' <ArithExp> ')'
    // cout << "In CalFactor()" << endl ; // debug��

    TokenDetail mTempCalToken ; // �Ȧs�@�ӭn�B�⪺Token

    mTempCalToken = mPassSyntaxVector[0] ; // ��vctor�̭����Ĥ@��Token
    // cout << mCalQueue.front().mTokenValue << endl ; // debug��
    mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vctor�̭����Ĥ@��Token

    if ( mTempCalToken.mTokenType == IDENT ) { // �Y�O�O�@��IDENT
      if ( InThemDefinedIDVectorOrNot( mTempCalToken.mToken ) != true ) { // �p�G�S���Q�w�q
        ThrowERROR3( mTempCalToken.mToken ) ; // ��X�ҥ~3
      } // if()
      else { // ���Q�Q�w�q�L�F��
        GetTheIDValue( mTempCalToken.mToken, value ) ; // ���o��ID����

        bool isfloat = false ;
        GetTheIDNumType( mTempCalToken.mToken, isfloat ) ;
        if ( isfloat == true ) {
          mIsfloat = true ;
        } // if()

        return ; // ����
      } // else
    } // if()
    else if ( mTempCalToken.mTokenType == POSIVITE ) { // �Y�O�O�@��POSITIVE
      mTempCalToken = mPassSyntaxVector[0] ; // ��vctor�̭����Ĥ@��Token(NUM)
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vctor�̭����Ĥ@��Token(NUM)or(FLNUM)

      if ( mTempCalToken.mTokenType == FLNUM ) {
        mIsfloat = true ;
        value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  �N string �� float
        return ; // ����
      } // if()

      // ����int�Ʀr
      value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  �N string �� float
      return ; // ����
    } // else if()
    else if ( mTempCalToken.mTokenType == NEGATIVE ) { // �Y�O�O�@��NEGATIVE
      mTempCalToken = mPassSyntaxVector[0] ; // ��vctor�̭����Ĥ@��Token(NUM)
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vctor�̭����Ĥ@��Token(NUM)(FLNUM)

      if ( mTempCalToken.mTokenType == FLNUM ) {
        mIsfloat = true ;
        value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  �N string �� float
        value = value * -1 ;
        return ; // ����
      } // if()

      value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  �N string �� float
      value = value * -1 ; // �t���Ʀr
      return ; // ����
    } // else if()
    else if ( mTempCalToken.mTokenType == NUM || mTempCalToken.mTokenType == FLNUM ) { // �Y�O�@��NUM(FLNUM)
      // �����Ʀr

      if ( mTempCalToken.mTokenType == FLNUM ) {
        mIsfloat = true ;
        value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  �N string �� float
        return ; // ����
      } // if()

      value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  �N string �� float
      return ; // ����
    } // else if()
    else if ( mTempCalToken.mTokenType == LP ) { // �Y�O�@�� ���A��LP
      float arithExpValue ;
      CalArithExp( arithExpValue ) ; // �I�sCalArithExp()
      value = arithExpValue ;
      mTempCalToken = mPassSyntaxVector[0] ; // ��vctor�̭����Ĥ@��Token(�k�A��)
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vctor�̭����Ĥ@��Token(�k�A��)
      return ;
    } // else if()
  } // CalFactor()

  void CalTerm( float &value ) { // �p��<Term>
    // cout << "In CalTerm()" << endl ; // debug��
    // <Term> ::= <Factor> { '*' <Factor> | '/' <Factor> }
    float cal1Factor1Value  ; // �Ψӱ����Ĥ@��Factor�Ҧ^�Ǫ���
    CalFactor( cal1Factor1Value ) ; // �I�sCalFactor()

    TokenDetail mTempCalToken ; // �Ȧs�@�ӭn�B�⪺Token

    mTempCalToken = mPassSyntaxVector[0] ; // ��vctor�̭����Ĥ@��Token

    // cout << mTempCalToken.mTokenValue << endl ; // debug��
    // �p�G�O* or /����
    while ( mTempCalToken.mTokenType == MULT || mTempCalToken.mTokenType == DIV ) {
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vctor�̭����Ĥ@��Token
      if (  mTempCalToken.mTokenType == MULT ) { // �O�ӭ��k
        float cal2FactorValue ; // �Ψӱ����ĤG��Factor�Ҧ^�Ǫ���
        CalFactor( cal2FactorValue ) ; // �I�sCalFactor()
        cal1Factor1Value = cal1Factor1Value * cal2FactorValue ;
      } // if()
      else { // ���k
        float cal2FactorValue ; // �Ψӱ�����2��Factor�Ҧ^�Ǫ���
        CalFactor( cal2FactorValue ) ; // �I�sCalFactor()
        if ( cal2FactorValue == 0 ) {
          ThrowERROR() ; // �ߥX�ҥ~
        } // if()
        else cal1Factor1Value = cal1Factor1Value / cal2FactorValue ; // else
      } // else

      mTempCalToken = mPassSyntaxVector[0] ; // ��vctor�̭����Ĥ@��Token
    } // while()

    value = cal1Factor1Value ; // �N�^�ǭȳ]�w
    return ;
  } // CalTerm()

  void CalArithExp( float &value ) { // �p��<ArithExp>
    // cout << "In CalArithExp()" << endl ; // debug��
    // <ArithExp> ::= <Term> { '+' <Term> | '-' <Term> }

    float calTerm1Value ; // �Ψӱ����Ĥ@��Term�Ҧ^�Ǫ���
    CalTerm( calTerm1Value ) ; // ���W�I�sCalTerm()

    TokenDetail mTempCalToken ; // �Ȧs�@�ӭn�B�⪺Token
    mTempCalToken = mPassSyntaxVector[0] ; // ��vctor�̭����Ĥ@��Token
    // �p�G�O+-����
    while ( mTempCalToken.mTokenType == ADD || mTempCalToken.mTokenType == SUB ) {
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vctor�̭����Ĥ@��Token
      if ( mTempCalToken.mTokenType == ADD ) { // �[�k
        float calTerm2Value ; // �Ψӱ�����2��Term�Ҧ^�Ǫ���
        CalTerm( calTerm2Value ) ; // ���W�I�sCalTerm()
        calTerm1Value = calTerm1Value + calTerm2Value ;
      } // if()
      else { // ��k
        float calTerm2Value ; // �Ψӱ�����2��Term�Ҧ^�Ǫ���
        CalTerm( calTerm2Value ) ; // ���W�I�sCalTerm()
        calTerm1Value = calTerm1Value - calTerm2Value ;
      } // else

      mTempCalToken = mPassSyntaxVector.front() ; // ��vctor�̭����Ĥ@��Token
    } // while()

    value = calTerm1Value ; // �]�w�^�ǭ�
    return ;
  } // CalArithExp()

  void CalBooleanOperator( bool &CalCorrect ) { // �p��<BooleanOperator>
    // cout << "In CalBooleanOperator()" << endl ; // debug��

    TokenDetail mTempCalToken ; // �Ȧs�@�ӭn�B�⪺Token
    mTempCalToken = mPassSyntaxVector[0] ; // ��vctor�̭����Ĥ@��Token
    if ( mTempCalToken.mTokenType != EQ && mTempCalToken.mTokenType != LG
         && mTempCalToken.mTokenType != G && mTempCalToken.mTokenType != L
         && mTempCalToken.mTokenType != GE && mTempCalToken.mTokenType != LE ) {
      CalCorrect = false  ; // �]�w�^�ǭ�
      return ;
    } // if()

    CalCorrect = true  ;
    return ;
  } // CalBooleanOperator()

  void CalNOT_ID_StartFactor( float &value ) { // �p��<NOT_ID_StartFactor>
    // cout << "In CalNOT_ID_StartFactor()" << endl ; // debug��
    // <NOT_ID_StartFactor> ::= [ SIGN ] NUM | '(' <ArithExp> ')'

    TokenDetail mTempCalToken ; // �Ȧs�@�ӭn�B�⪺Token
    mTempCalToken = mPassSyntaxVector[0] ; // ��vctor�̭����Ĥ@��Token
    // cout << mCalQueue.front().mTokenValue << endl ; // debug��
    if ( mTempCalToken.mTokenType == POSIVITE ) { // �Y�O�O�@��POSITIVE
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vctor�̭����Ĥ@��Token(�OPOSITIVE)
      mTempCalToken = mPassSyntaxVector[0] ; // ��vctor�̭����Ĥ@��Token(�ONUM)
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vctor�̭����Ĥ@��Token(�ONUM)

      if ( mTempCalToken.mTokenType == FLNUM ) {
        mIsfloat = true ;
        value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  �N string �� float
        return ;
      } // if()

      value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  �N string �� float
      return ; // ����
    } // if()
    else if ( mTempCalToken.mTokenType == NEGATIVE ) { // �Y�O�O�@��NEGATIVE
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vctor�̭����Ĥ@��Token(�ONEGATIVE)
      mTempCalToken = mPassSyntaxVector[0] ; // ��vctor�̭����Ĥ@��Token(�ONUM)
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vctor�̭����Ĥ@��Token(�ONUM)

      if ( mTempCalToken.mTokenType == FLNUM ) {
        mIsfloat = true ;
        value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  �N string �� float
        value = value * -1 ;
        return ; // ����
      } // if()

      value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  �N string �� float
      value = value * -1 ;
      return ; // ����
    } // else if()
    else if ( mTempCalToken.mTokenType == NUM || mTempCalToken.mTokenType == FLNUM ) { // �Y�O�@��NUM
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vctor�̭����Ĥ@��Token(�ONUM)

      if ( mTempCalToken.mTokenType == FLNUM ) {
        mIsfloat = true ;
        value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  �N string �� float
        return ; // ����
      } // if()

      value = ( float ) atof( mTempCalToken.mToken.c_str() ) ; //  �N string �� float
      return ;
    } // else if()
    else if ( mTempCalToken.mTokenType == LP ) { // �Y�O�@�� ���A��LP
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vctor�̭����Ĥ@��Token(�OLP)
      float arithExpValue ;
      CalArithExp( arithExpValue ) ; // �I�sCalArithExp()
      value = arithExpValue ;
      mTempCalToken = mPassSyntaxVector[0] ; // ��vctor�̭����Ĥ@��Token(�k�A��)
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vctor�̭����Ĥ@��Token(�ORP)
      return ;
    } // else if
    // �p�G���򳣨S�i�h
  } // CalNOT_ID_StartFactor()

  void CalNOT_ID_StartTerm( float &value ) { // �p��<NOT_ID_StartTerm>
    // cout << "In CalNOT_ID_StartTerm()" << endl ; // debug��
    // <NOT_ID_StartTerm> ::= <NOT_ID_StartFactor> { '*' <Factor> | '/' <Factor> }

    float calNOT_ID_StartFactorValue ; // �Ψӱ����Ĥ@��NOT_ID_StartFactor()�Ҧ^�Ǫ���
    CalNOT_ID_StartFactor( calNOT_ID_StartFactorValue ) ; // �I�sNOT_ID_StartFactor()

    TokenDetail mTempCalToken ; // �Ȧs�@�ӭn�B�⪺Token
    mTempCalToken = mPassSyntaxVector[0] ; // ��vctor�̭����Ĥ@��Token
    // cout << mTempCalToken.mTokenValue << endl ;
    // �p�G�O* or /����
    while ( mTempCalToken.mTokenType == MULT || mTempCalToken.mTokenType == DIV ) {
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vctor�̭����Ĥ@��Token
      if (  mTempCalToken.mTokenType == MULT ) { // ���k
        float calFactorValue ; // �Ψӱ���CalFactor����
        CalFactor( calFactorValue ) ; // �I�sCalFactor
        calNOT_ID_StartFactorValue = calNOT_ID_StartFactorValue * calFactorValue ;
      } // if()
      else { // ���k
        float calFactorValue ; // �Ψӱ���CalFactor����
        CalFactor( calFactorValue ) ; // �I�sCalFactor
        if ( calFactorValue == 0 ) {
          // cout << "throw error from CalNOT_ID_StartTerm " << endl ; // debug��
          ThrowERROR() ; // �ߥX�ҥ~
        } // if()

        calNOT_ID_StartFactorValue = calNOT_ID_StartFactorValue / calFactorValue ;
      } // else

      mTempCalToken = mPassSyntaxVector[0] ; // ��vctor�̭����Ĥ@��Token
    } // while()

    value = calNOT_ID_StartFactorValue ; // �]�w�^�ǭ�
    return ;
  } // CalNOT_ID_StartTerm()

  void CalNOT_ID_StartArithExp( float &value ) { // �p��<NOT_ID_StartArithExp>
    // cout << "In CalNOT_ID_StartArithExp()" << endl ; // debug��
    // <NOT_ID_StartArithExp>  ::= <NOT_ID_StartTerm> { '+' <Term> | '-' <Term> }

    float calNOT_ID_StartTermValue ; // �Ψӱ����Ĥ@��CalNOT_ID_StartTerm()�Ҧ^�Ǫ���
    CalNOT_ID_StartTerm( calNOT_ID_StartTermValue ) ; // ���W�I�sCalNOT_ID_StartTerm()

    if ( mPassSyntaxVector.empty() ) { // �p�G�ŤF�N�����~��
      value = calNOT_ID_StartTermValue ;
      return ;
    }  // if()

    TokenDetail mTempCalToken ; // �Ȧs�@�ӭn�B�⪺Token
    mTempCalToken = mPassSyntaxVector[0] ; // ��vctor�̭����Ĥ@��Token
    // �p�G�O+-
    while ( mTempCalToken.mTokenType == ADD || mTempCalToken.mTokenType == SUB ) {
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vctor�̭����Ĥ@��Token
      if (  mTempCalToken.mTokenType == ADD ) { // �[�k
        float calTermValue ; // �Ψӱ����Ĥ@��CalTerm()�Ҧ^�Ǫ���
        CalTerm( calTermValue ) ; // ���W�I�sCalTerm()
        calNOT_ID_StartTermValue = calNOT_ID_StartTermValue + calTermValue ;
      } // if()
      else { // ��k
        float calTermValue ; // �Ψӱ����Ĥ@��CalTerm()�Ҧ^�Ǫ���
        CalTerm( calTermValue ) ; // ���W�I�sCalTerm()
        calNOT_ID_StartTermValue = calNOT_ID_StartTermValue - calTermValue ;
      } // else

      mTempCalToken = mPassSyntaxVector[0] ; // ��vctor�̭����Ĥ@��Token
    } // while()

    value = calNOT_ID_StartTermValue ; // �]�w�^�ǭ�
    return ;

  } // CalNOT_ID_StartArithExp()

  void CalNOT_ID_StartArithExpOrBexp( float &value ) { // �p��<NOT_ID_StartArithExpOrBexp>
    // cout << "In CalNOT_ID_StartArithExpOrBexp()" << endl ; // debug��
    // <NOT_ID_StartArithExpOrBexp> ::= <NOT_ID_StartArithExp> [ <BooleanOperator> <ArithExp> ]

    float calNOT_ID_StartArithExpValue ; // �Ψӱ���NOT_ID_StartArithExp()����
    CalNOT_ID_StartArithExp( calNOT_ID_StartArithExpValue ) ; // �I�sNOT_ID_StartArithExp()
    // �즹���� �ڦܤ֦�<NOT_ID_StartArithExp>����

    bool nextIsBool = false ; // �P�_���S���ݭn��[ <BooleanOperator> <ArithExp> ]
    CalBooleanOperator( nextIsBool ) ; // �ݬݤU�@�ӬO���OBooleanOperator�p�G�O���@�w�]��ArithExp

    if ( nextIsBool != true ) {
      value  = calNOT_ID_StartArithExpValue ;
      if ( mIsfloat == true ) printf( "> %1.3f\n", value ) ;
      else {
        int answer ;
        answer = value ;
        cout << "> " << answer << endl ;
        // printf( "> %d\n", value ) ;
      } // else
      // ��X���׬��@�ӭȦ]��[]�S���F��n��(���G)
      return ;
    } // if()
    else { // �OBooleanOperator���@�w�]��ArithExp
      TokenDetail mTempCalToken ; // �Ȧs�@�ӭn�B�⪺Token
      mTempCalToken = mPassSyntaxVector[0] ; // ��vctor�̭����Ĥ@��Token(���L)
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vctor�̭����Ĥ@��Token(���L)
      TOKENTYPE nowBoolType = mTempCalToken.mTokenType ; // �N�ثe�����L�O���U��

      float calArithExpValue ; // �Ψӱ���CalArithExp()����
      CalArithExp( calArithExpValue ) ; // �I�sCalArithExp(

      if ( nowBoolType == EQ ) { // �P�_�O�_����n�ϥ�TOLERANCE
        if ( calNOT_ID_StartArithExpValue == calArithExpValue ) {
          cout << "> true" << endl ; // (���G)
          return ;
        } // if
        else {
          cout << "> false" << endl ; // (���G)
          return ;
        } // else
      } // if()
      else if ( nowBoolType == LG ) { // �]�N�O�ݬۤ��۵�
        if ( calNOT_ID_StartArithExpValue != calArithExpValue ) {
          cout << "> true" << endl ; // (���G)
          return ;
        } // if
        else {
          cout << "> false" << endl ; // (���G)
          return ;
        } // else
      } // else if()
      else if ( nowBoolType == G ) { // �n�j�W�LTOLERANCE
        if ( calNOT_ID_StartArithExpValue > calArithExpValue ) {
          cout << "> true" << endl ; // (���G)
          return ;
        } // if()
        else {
          cout << "> false" << endl ; // (���G)
          return ;
        } // else
      } // else if()
      else if ( nowBoolType == L ) { // �n�p�W�LTOLERANCE
        if ( calNOT_ID_StartArithExpValue < calArithExpValue ) {
          cout << "> true" << endl ; // (���G)
          return ;
        } // if()
        else {
          cout << "> false" << endl ; // (���G)
          return ;
        } // else
      } // else if()
      else if ( nowBoolType == GE ) { // �O�۵��Ϊ̤j��
        if ( calNOT_ID_StartArithExpValue >= calArithExpValue ) {
          cout << "> true" << endl ; // (���G)
          return ;
        } // if
        else {
          cout << "> false" << endl ; // (���G)
          return ;
        } // else
      } // else if()
      else if ( nowBoolType == LE ) { // �O�۵��Ϊ̤p��
        if ( calNOT_ID_StartArithExpValue <= calArithExpValue ) {
          cout << "> true" << endl ; // (���G)
          return ;
        } // if
        else {
          cout << "> false" << endl ; // (���G)
          return ;
        } // else
      } // else if()
    } // else()
  } // CalNOT_ID_StartArithExpOrBexp()

  void CalIDlessArithExpOrBexp( string IDname, float &value ) { // �p��<IDlessArithExpOrBexp>
    // cout << "In CalIDlessArithExpOrBexp()" << endl ; // debug��
    // <IDlessArithExpOrBexp>  ::= {   '+' <Term>   | '-' <Term>
    // | '*' <Factor> | '/' <Factor>
    // }
    // [ <BooleanOperator> <ArithExp> ]

    if ( InThemDefinedIDVectorOrNot( IDname ) != true )  { // ���ݬݱqcommand�Ӫ���ID�O�_�w�Q�w�q;
      ThrowERROR3( IDname ) ;
    } // if()
    else { // ID���Q�w�q
      float answer ;
      GetTheIDValue( IDname, answer ) ; // ������ID����
      TokenDetail mTempCalToken ; // �Ȧs�@�ӭn�B�⪺Token

      bool isfloat = false ;
      GetTheIDNumType( IDname, isfloat ) ;
      if ( isfloat == true ) {
        mIsfloat = true ;
      } // if()

      mTempCalToken = mPassSyntaxVector[0] ; // ��Vector�̭����Ĥ@��Token
      while ( mTempCalToken.mTokenType == ADD || mTempCalToken.mTokenType == SUB
              || mTempCalToken.mTokenType == MULT || mTempCalToken.mTokenType == DIV ) {
        mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vctor�̭����Ĥ@��Token
        if (  mTempCalToken.mTokenType == ADD ) { // �[�k
          float calTermValue ; // �ΨӦs��CalTerm()����
          CalTerm( calTermValue ) ; // �I�sCalTerm()
          answer = answer + calTermValue ;
        } // if()
        else if ( mTempCalToken.mTokenType == SUB ) { // ��k
          float calTermValue ; // �ΨӦs��CalTerm()����
          CalTerm( calTermValue ) ; // �I�sCalTerm()
          answer = answer - calTermValue ;
        } // else if
        else if (  mTempCalToken.mTokenType == MULT ) { // ���k
          float calFactorValue ; // �Ψӱ���CalFactor����
          CalFactor( calFactorValue ) ; // �I�sCalFactor
          answer = answer * calFactorValue ;
        } // else if()
        else { // ���k
          float calFactorValue ; // �Ψӱ���CalFactor����
          CalFactor( calFactorValue ) ; // �I�sCalFactor
          if ( calFactorValue == 0 ) {
            ThrowERROR() ;
          } // if

          answer = answer / calFactorValue ;
        } // else

        mTempCalToken = mPassSyntaxVector[0] ; // ��Vector�̭����Ĥ@��Token

        // mTempCalToken = mCalQueue.front() ; // �~���QUEUE�̭����Ĥ@��Token
      } // while()

      value = answer ; // �]�w���G��

      bool nextIsBool = false ; // �P�_�n���n���᭱��[ <BooleanOperator> <ArithExp> ]
      CalBooleanOperator( nextIsBool ) ; // �ݬݤU�@�ӬO���OBooleanOperator�p�G�O���@�w�]��ArithExp

      if ( nextIsBool != true ) { // �Y�U�@�ӪF�褣�Obool
        if ( mIsfloat == true ) printf( "> %1.3f\n", value ) ;
        else {
          int answer ;
          answer = value ;
          cout << "> " << answer << endl ;
          // printf( "> %d\n", value ) ;
        } // else
        // ��X���׬��@�ӭȦ]��[]�S���F��n��(���G)
        return ;
      } // if()
      else { // �OBooleanOperator���@�w�]��ArithExp
        TokenDetail mTempCalToken ; // �Ȧs�@�ӭn�B�⪺Token
        mTempCalToken = mPassSyntaxVector[0] ; // ��QUEUE�̭����Ĥ@��Token(���L)
        mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vctor�̭����Ĥ@��Token
        TOKENTYPE nowBoolType = mTempCalToken.mTokenType ; // �N�ثe�����L�O���U��

        float calArithExpValue ; // �ΨӦs��CalArithExp����
        CalArithExp( calArithExpValue ) ; // �I�sCalArithExp()

        if ( nowBoolType == EQ ) { // �P�_�O�_����n�ϥ�TOLERANCE
          if ( value == calArithExpValue ) {
            cout << "> true" << endl ; // (���G)
            return ;
          } // if
          else {
            cout << "> false" << endl ; // (���G)
            return ;
          } // else
        } // if()
        else if ( nowBoolType == LG ) { // �]�N�O�ݬۤ��۵�
          if ( value != calArithExpValue ) {
            cout << "> true" << endl ; // (���G)
            return ;
          } // if
          else {
            cout << "> false" << endl ; // (���G)
            return ;
          } // else
        } // else if()
        else if ( nowBoolType == G ) { // �Y�j-�p�W�LTOLERANCE�h�����j��
          if ( value > calArithExpValue  ) {
            cout << "> true" << endl ; // (���G)
            return ;
          } // if()
          else {
            cout << "> false" << endl ; // (���G)
            return ;
          } // else
        } // else if()
        else if ( nowBoolType == L ) { // �Y�p�M�j���t�Z�W�LTOLERANCE�h�����p��
          if ( value < calArithExpValue ) {
            cout << "> true" << endl ; // (���G)
            return ;
          } // if()
          else {
            cout << "> false" << endl ; // (���G)
            return ;
          } // else
        } // else if()
        else if ( nowBoolType == GE ) { // �j��ε���
          if ( value >= calArithExpValue ) {
            cout << "> true" << endl ; // (���G)
            return ;
          } // if
          else {
            cout << "> false" << endl ; // (���G)
            return ;
          } // else
        } // else if()
        else if ( nowBoolType == LE ) { // �p��ε���
          if ( value <= calArithExpValue ) {
            cout << "> true" << endl ; // (���G)
            return ;
          } // if
          else {
            cout << "> false" << endl ; // (���G)
            return ;
          } // else
        } // else if()
      } // else()
    } // else
  } // CalIDlessArithExpOrBexp()

  void CalCommand( float &value ) { // �p��<CalCommand>�I�s�Ҧ�function���|�D�ʧ����Ʊ�
    // cout << "In CalCommand()" << endl ; // debug��
    // <Command> ::= IDENT ( ':=' <ArithExp> | <IDlessArithExpOrBexp> ) ';'
    // | <NOT_IDStartArithExpOrBexp> ';'
    // | QUIT

    TokenDetail mTempCalToken ; // �Ȧs�@�ӭn�B�⪺Token
    mTempCalToken = mPassSyntaxVector.front() ; // ��vector�̭��a�@��token
    // �p�G�OID
    if ( mTempCalToken.mTokenType == IDENT ) {
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vector�̭��a�@��token

      IDDetail mTempID ; // �Ȧs�@��ID
      mTempID.mIDName = mTempCalToken.mToken ; // ID�W�r����token����
      mTempID.mIDTokenLine = mTempCalToken.mTokenLine ; // ID��LINE����token��LINE

      mTempCalToken = mPassSyntaxVector.front() ; // ��vector�̭��a�@��token
      if ( mTempCalToken.mTokenType == ASSIGN ) { // �p�G�O:=
        mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vector�̭��a�@��token(:=)

        float calArithExpValue ; // �I�sCalArithExp�Ө��o�ƭ�
        CalArithExp( calArithExpValue ) ; // �I�sCalArithExp�Ө��o�ƭ�
        mTempID.mIDisfloat = mIsfloat ;
        // ���۬ݬݦ��S���o��ID�s�b
        if ( InThemDefinedIDVectorOrNot( mTempID.mIDName ) == true ) { // �P�_���S����ID
          SetTheIDValue( mTempID.mIDName, calArithExpValue ) ; // �h��s��ID����
          SetTheIDNumType( mTempID.mIDName, mIsfloat ) ; // ��s��ID��NUMTYPE
          if ( mIsfloat == true ) printf( "> %1.3f\n", calArithExpValue ) ;
          else {
            int answer ;
            answer = calArithExpValue ;
            cout << "> " << answer << endl ;
            // printf( "> %d\n", calArithExpValue ) ;
          } // else
        } // if()
        else { // �p�G�O�@�ӷs��ID�����O���s�W�@�ӷs��ID
          mTempID.mIDValue = calArithExpValue ; // �]�wID����
          // cout << mTempID.misfloat << endl ;
          PushInTomDefinedIDVector( mTempID ) ; // �hVECTOR�s�W�w�q�o��ID
          if ( mIsfloat == true ) printf( "> %1.3f\n", calArithExpValue ) ;
          else {
            int answer ;
            answer = calArithExpValue ;
            cout << "> " << answer << endl ;
            // printf( "> %d\n", calArithExpValue ) ;
          } // else
        } // else

        mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vector�̭��a�@��token(;)
        return ; // ����
      } // if()
      else { // ID�᭱���O:=
        // ��ID�W�r��<IDlessArithExpOrBexp>
        CalIDlessArithExpOrBexp( mTempID.mIDName, value ) ; // �w�bCalIDlessArithExpOrBexp()����cout
        mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vector�̭��a�@��token(;)
        return ;
      } // else
    } // if()
    else { // �I�s <NOT_IDStartArithExpOrBexp>
      CalNOT_ID_StartArithExpOrBexp( value ) ; // ��X�w�bCalNOT_ID_StartArithExpOrBexp()����
      mPassSyntaxVector.erase( mPassSyntaxVector.begin() ) ; // �R��vector�̭��a�@��token(;)
      return ;
    } // else
  } // CalCommand()

  void SyntaxAnalysis() { // ��k���R
    // cout << "into SyntaxAnalysis()" << endl ; // debug��

    do { // �YToken�S�F
      try {
        TokenDetail mTempToken ; // �Ȧs
        Command() ; // �I�sCommand()
        float value ;
        CalCommand( value ) ;
      } // try
      catch ( string ErrorMsg ) { // �Ҧ����~!
        cout << "> " << ErrorMsg << endl ;
        mScanner.ClearToEnter() ; // Ū�촫�欰��A�٨S��J�����n���˱�
      } // catch
      catch ( TOKENTYPE type ) { // ���_���O
        mIsfloat = false ; // ���s
        if ( type == QUIT ) return ;
      } // catch

      mScanner.ClearmPeekToken() ;
      mPassSyntaxVector.clear() ;
      mIsfloat = false ; // ���s

    } while ( true ) ;
  } // SyntaxAnalysis()

}; // class Parser

int main()
{
  cout << "Program starts..." << endl ;
  cin >> uTestNumber ; // ���ռƾ��D��

  Scanner scanner ;
  Parser parser( scanner ) ;
  // parser.Debug2() ;
  parser.SyntaxAnalysis() ;
  // if ( uTestNumber == 2 ) cout << "YA" << endl ;
  // system("pause");
  cout << "> " << "Program exits..." << endl ;
  // return 0 ;
} // main()


