/*******************************************************
Copyright (C) 2013 James Higley
Copyright (C) 2013 Stefano Giorgio

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Placeuite 330, Boston, MA  02111-1307  USA
 ********************************************************/

#include "defs.h"
#include <cppunit/config/SourcePrefix.h>
#include "cpu_timer.h"
#include "db_init_model.h"
#include "framebase_tests.h"
//----------------------------------------------------------------------------
#include "test_assets.h"
#include "assetspanel.h"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(Test_Asset);

static int s_instance_count = 0;
//----------------------------------------------------------------------------
Test_Asset::Test_Asset()
{
    s_instance_count++;
    m_this_instance = s_instance_count;
    m_test_db_filename = "test_db_model_asset.mmb";
}

Test_Asset::~Test_Asset()
{
    s_instance_count--;
    if (s_instance_count < 1)
    {
        wxRemoveFile(m_test_db_filename);
    }
}

void Test_Asset::setUp()
{
    CpuTimer time("Startup");
    m_test_db.Open(m_test_db_filename);
    m_frame = new TestFrameBase(m_this_instance);
    m_frame->Show(true);

    m_dbmodel = new DB_Init_Model();
    m_dbmodel->Init_Model_Assets(&m_test_db);
}

void Test_Asset::tearDown()
{
    m_test_db.Close();
    delete m_frame;
    delete m_dbmodel;
}

void Test_Asset::test_add()
{
    CpuTimer time("add");
    wxDateTime asset_date = wxDateTime::Today().Subtract(wxDateSpan::Years(5));
    int asset_id = m_dbmodel->Add_Asset("Stop Watch", asset_date, 1000, Model_Asset::TYPE_JEWELLERY, Model_Asset::RATE_APPRECIATE, 20.0, "Appreciate 20% pa 5 years");
    CPPUNIT_ASSERT(asset_id == 1);

    asset_id = m_dbmodel->Add_Asset("Stop Watch", asset_date, 1000, Model_Asset::TYPE_JEWELLERY, Model_Asset::RATE_DEPRECIATE, 20.0, "Depreciates 20% pa 5 years");
    CPPUNIT_ASSERT(asset_id == 2);

    asset_id = m_dbmodel->Add_Asset("Stop Watch", asset_date, 1000, Model_Asset::TYPE_JEWELLERY, Model_Asset::RATE_NONE, 20.0, "No change over 5 years");
    CPPUNIT_ASSERT(asset_id == 3);

    asset_id = m_dbmodel->Add_Asset("To be deleted", asset_date, 1000, Model_Asset::TYPE_JEWELLERY, Model_Asset::RATE_APPRECIATE, 20.0, "Appreciate 20% pa 5 years");
    CPPUNIT_ASSERT(asset_id == 4);

    Model_Asset::Data_Set assets = Model_Asset::instance().all();
    CPPUNIT_ASSERT(assets.size() == 4);
}

void Test_Asset::test_appreciate()
{
    Model_Asset::Data_Set asset_list = Model_Asset::instance().all();
    Model_Asset::Data entry = asset_list[0];

    double value = Model_Asset::value(entry);

    CPPUNIT_ASSERT(entry.VALUECHANGE == "Appreciates");
    CPPUNIT_ASSERT((value > 2488) && (value < 2489));       // values from V 0.9.9.0
    //CPPUNIT_ASSERT((value > 2000) && (value < 2001));     // values from v 0.9.9.2
}

void Test_Asset::test_depreciate()
{
    Model_Asset::Data_Set asset_list = Model_Asset::instance().all();
    Model_Asset::Data entry = asset_list[1];

    double value = Model_Asset::value(entry);

    CPPUNIT_ASSERT(entry.VALUECHANGE == "Depreciates");
    CPPUNIT_ASSERT((value > 327) && (value < 328));     // values from V 0.9.9.0         
    //CPPUNIT_ASSERT(value == 0);                       // values from v 0.9.9.2
}

void Test_Asset::test_remove()
{
    Model_Asset::instance().remove(4);
    Model_Asset::Data_Set assets = Model_Asset::instance().all();
    CPPUNIT_ASSERT(assets.size() == 3);
}

void Test_Asset::test_assetpanel()
{
    // Create a new frame anchored to the base frame.
    TestFrameBase* my_frame = new TestFrameBase(m_frame, 670, 400);
    my_frame->Show();

    // Create the panel under test
    mmAssetsPanel mypanel = new mmAssetsPanel(my_frame);
    mypanel.Show();

    // Anchor the panel. Otherwise it will disappear. 
    m_dbmodel->ShowMessage("Examine the Asset Panel.\n\nContinue other tests...");
}
//--------------------------------------------------------------------------