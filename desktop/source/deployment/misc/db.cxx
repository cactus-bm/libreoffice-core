/*************************************************************************
 *
 *  $RCSfile: db.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2005-03-30 08:44:06 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#include <db.hxx>

#include <rtl/alloc.h>
#include <cstring>
#include <errno.h>

namespace berkeleydbproxy {

//----------------------------------------------------------------------------
    namespace db_internal
    {
        static void raise_error(int dberr, const char * where);

        static inline int check_error(int dberr, const char * where)
        {
            if (dberr) raise_error(dberr,where);
            return dberr;
        }
    }

//----------------------------------------------------------------------------

DbEnv::DbEnv(u_int32_t flags)
: m_pDBENV(0)
{
    db_internal::check_error( db_env_create(&m_pDBENV,flags), "DbEnv::DbEnv" );
}

DbEnv::~DbEnv()
{
    if (m_pDBENV)
    {
        // should not happen
        // TODO: add assert
        m_pDBENV->close(m_pDBENV,0);
    }
}

int DbEnv::open(const char *db_home, u_int32_t flags, int mode)
{
  return db_internal::check_error( m_pDBENV->open(m_pDBENV,db_home,flags,mode), "DbEnv::open" );
}

void DbEnv::close(u_int32_t flags)
{
    int error = m_pDBENV->close(m_pDBENV,flags);
    m_pDBENV = 0;

    db_internal::check_error(error, "DbEnv::close");
}

int DbEnv::set_alloc(db_malloc_fcn_type app_malloc,
                     db_realloc_fcn_type app_realloc,
                     db_free_fcn_type app_free)
{
    int err = m_pDBENV->set_alloc(m_pDBENV,app_malloc,app_realloc,app_free);
    return db_internal::check_error(err,"Db::set_alloc");
}


char *DbEnv::strerror(int error) {
    return (db_strerror(error));
}

//----------------------------------------------------------------------------

int Db::set_alloc(   db_malloc_fcn_type app_malloc,
                     db_realloc_fcn_type app_realloc,
                     db_free_fcn_type app_free)
{
    int err = m_pDBP->set_alloc(m_pDBP,app_malloc,app_realloc,app_free);
    return db_internal::check_error(err,"Db::set_alloc");
}

Db::Db(DbEnv* pDbenv,u_int32_t flags)
: m_pDBP(0)
{
    db_internal::check_error( db_create(&m_pDBP,pDbenv ? pDbenv->m_pDBENV:0,flags),"Db::Db" );
}


Db::~Db()
{
    if (m_pDBP)
    {
        // should not happen
        // TODO: add assert
    }

}


int Db::close(u_int32_t flags)
{
    int error = m_pDBP->close(m_pDBP,flags);
    m_pDBP = 0;
    return db_internal::check_error(error,"Db::close");
}

int Db::open(DB_TXN *txnid,
             const char *file,
             const char *database,
             DBTYPE type,
             u_int32_t flags,
             int mode)
{
    int err = m_pDBP->open(m_pDBP,txnid,file,database,type,flags,mode);
    return db_internal::check_error( err,"Db::open" );
}


int Db::get(DB_TXN *txnid, Dbt *key, Dbt *data, u_int32_t flags)
{
    int err = m_pDBP->get(m_pDBP,txnid,key,data,flags);

    // these are non-exceptional outcomes
    if (err != DB_NOTFOUND && err != DB_KEYEMPTY)
        db_internal::check_error( err,"Db::get" );

    return err;
}

int Db::put(DB_TXN* txnid, Dbt *key, Dbt *data, u_int32_t flags)
{
    int err = m_pDBP->put(m_pDBP,txnid,key,data,flags);

    if (err != DB_KEYEXIST) // this is a non-exceptional outcome
        db_internal::check_error( err,"Db::put" );
    return err;
}

int Db::cursor(DB_TXN *txnid, Dbc **cursorp, u_int32_t flags)
{
    DBC * dbc = 0;
    int error = m_pDBP->cursor(m_pDBP,txnid,&dbc,flags);

    if (!db_internal::check_error(error,"Db::cursor"))
        *cursorp = new Dbc(dbc);

    return error;
}


#define DB_INCOMPLETE           (-30999)/* Sync didn't finish. */

int Db::sync(u_int32_t flags)
{
    int err;
    DB *db = m_pDBP;

    if (!db) {
        db_internal::check_error(EINVAL,"Db::sync");
        return (EINVAL);
    }
    if ((err = db->sync(db, flags)) != 0 && err != DB_INCOMPLETE) {
        db_internal::check_error(err, "Db::sync");
        return (err);
    }
    return (err);
}

int Db::del(Dbt *key, u_int32_t flags)
{
    DB *db = m_pDBP;
    int err;

    if ((err = db->del(db, 0, key, flags)) != 0) {
        // DB_NOTFOUND is a "normal" return, so should not be
        // thrown as an error
        //
        if (err != DB_NOTFOUND) {
            db_internal::check_error(err, "Db::del");
            return (err);
        }
    }
    return (err);
}

//----------------------------------------------------------------------------

Dbc::Dbc(DBC * dbc)
: m_pDBC(dbc)
{
}

Dbc::~Dbc()
{
}

int Dbc::close()
{
    int err = m_pDBC->c_close(m_pDBC);
    delete this;
    return db_internal::check_error( err,"Dbcursor::close" );
}

int Dbc::get(Dbt *key, Dbt *data, u_int32_t flags)
{
    int err = m_pDBC->c_get(m_pDBC,key,data,flags);

    // these are non-exceptional outcomes
    if (err != DB_NOTFOUND && err != DB_KEYEMPTY)
        db_internal::check_error( err, "Dbcursor::get" );

    return err;
}

int Dbc::del(u_int32_t flags_arg)
{
    DBC *cursor = m_pDBC;
    int err;

    if ((err = cursor->c_del(cursor, flags_arg)) != 0) {

        // DB_KEYEMPTY is a "normal" return, so should not be
        // thrown as an error
        //
        if (err != DB_KEYEMPTY) {
            db_internal::check_error( err, "Db::del");
            return (err);
        }
    }
    return (err);
}

//----------------------------------------------------------------------------


Dbt::Dbt()
{
    using namespace std;
    DBT * thispod = this;
    memset(thispod, 0, sizeof *thispod);
}


Dbt::Dbt(void *data_arg, u_int32_t size_arg)
{
    using namespace std;
    DBT * thispod = this;
    memset(thispod, 0, sizeof *thispod);
    this->set_data(data_arg);
    this->set_size(size_arg);
}

Dbt::Dbt(const Dbt & other)
{
    using namespace std;
    const DBT *otherpod = &other;
    DBT *thispod = this;
    memcpy(thispod, otherpod, sizeof *thispod);
}

Dbt& Dbt::operator = (const Dbt & other)
{
    if (this != &other)
    {
        using namespace std;
        const DBT *otherpod = &other;
        DBT *thispod = this;
        memcpy(thispod, otherpod, sizeof *thispod);
    }
    return *this;
}

Dbt::~Dbt()
{
}

void * Dbt::get_data() const
{
    return this->data;
}

void Dbt::set_data(void *value)
{
    this->data = value;
}

u_int32_t Dbt::get_size() const
{
    return this->size;
}

void Dbt::set_size(u_int32_t value)
{
    this->size = value;
}

void Dbt::set_flags(u_int32_t value)
{
    this->flags = value;
}

//----------------------------------------------------------------------------
void db_internal::raise_error(int dberr, const char * where)
{
    if (!where) where = "<unknown>";

    const char * dberrmsg = db_strerror(dberr);
    if (!dberrmsg || !*dberrmsg) dberrmsg = "<unknown DB error>";

    rtl::OString msg = where;
    msg += ": ";
    msg += dberrmsg;

    throw DbException(msg);
}

//----------------------------------------------------------------------------
} // namespace ecomp

