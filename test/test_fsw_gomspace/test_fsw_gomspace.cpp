#include "../StateFieldRegistryMock.hpp"
#include "../../src/FCCode/GomspaceController.hpp"

#include <unity.h>

class TestFixture {
  public:
    StateFieldRegistryMock registry;

    Devices::Gomspace::eps_hk_t hk = {
        {1,2,3},
        4,
        {5,6,7},
        8,
        9,
        10,
        {11,12,13,14,15,16},
        {true, false, true, false, true, false, true, false},
        {25,26,27,28,29,30,31,32},
        {33,34,35,36,37,38,39,40},
        {41,42,43,44,45,46},
        47,
        48,
        {49,50},
        51,
        52,
        {53,54},
        55,
        {56,57,58,59},
        62,
        63,
        64,
        65
    };
    Devices::Gomspace::eps_config_t config;
    Devices::Gomspace::eps_config2_t config2;
    Devices::Gomspace gs;

    std::unique_ptr<GomspaceController> gs_controller;

    ReadableStateField<unsigned int>* vboost1_fp;
    ReadableStateField<unsigned int>* vboost2_fp;
    ReadableStateField<unsigned int>* vboost3_fp;

    ReadableStateField<unsigned int>* vbatt_fp;

    ReadableStateField<unsigned int>* curin1_fp;
    ReadableStateField<unsigned int>* curin2_fp;
    ReadableStateField<unsigned int>* curin3_fp;

    ReadableStateField<unsigned int>* cursun_fp;

    ReadableStateField<unsigned int>* cursys_fp;

    ReadableStateField<unsigned int>* curout1_fp;
    ReadableStateField<unsigned int>* curout2_fp;
    ReadableStateField<unsigned int>* curout3_fp;
    ReadableStateField<unsigned int>* curout4_fp;
    ReadableStateField<unsigned int>* curout5_fp;
    ReadableStateField<unsigned int>* curout6_fp;

    ReadableStateField<bool>* output1_fp;
    ReadableStateField<bool>* output2_fp;
    ReadableStateField<bool>* output3_fp;
    ReadableStateField<bool>* output4_fp;
    ReadableStateField<bool>* output5_fp;
    ReadableStateField<bool>* output6_fp;

    ReadableStateField<unsigned int>* wdt_i2c_time_left_fp;

    ReadableStateField<unsigned int>* counter_wdt_i2c_fp;

    ReadableStateField<unsigned int>* counter_wdt_gnd_fp;

    ReadableStateField<unsigned int>* counter_wdt_csp1_fp;
    ReadableStateField<unsigned int>* counter_wdt_csp2_fp;

    ReadableStateField<unsigned int>* counter_boot_fp;

    ReadableStateField<signed int>* temp1_fp;
    ReadableStateField<signed int>* temp2_fp;
    ReadableStateField<signed int>* temp3_fp;
    ReadableStateField<signed int>* temp4_fp;

    ReadableStateField<unsigned char>* bootcause_fp;

    ReadableStateField<unsigned char>* battmode_fp;

    ReadableStateField<unsigned char>* pptmode_fp;

    TestFixture() : registry(), gs(&hk, &config, &config2) {
        gs_controller = std::make_unique<GomspaceController>(registry, 0, gs);

        vboost1_fp = registry.find_readable_field_t<unsigned int>("gomspace.vboost.output1");
        vboost2_fp = registry.find_readable_field_t<unsigned int>("gomspace.vboost.output2");
        vboost3_fp = registry.find_readable_field_t<unsigned int>("gomspace.vboost.output3");

        vbatt_fp = registry.find_readable_field_t<unsigned int>("gomspace.vbatt");

        curin1_fp = registry.find_readable_field_t<unsigned int>("gomspace.curin.output1");
        curin2_fp = registry.find_readable_field_t<unsigned int>("gomspace.curin.output2");
        curin3_fp = registry.find_readable_field_t<unsigned int>("gomspace.curin.output3");

        cursun_fp = registry.find_readable_field_t<unsigned int>("gomspace.cursun");
        cursys_fp = registry.find_readable_field_t<unsigned int>("gomspace.cursys");

        curout1_fp = registry.find_readable_field_t<unsigned int>("gomspace.curout.output1");
        curout2_fp = registry.find_readable_field_t<unsigned int>("gomspace.curout.output2");
        curout3_fp = registry.find_readable_field_t<unsigned int>("gomspace.curout.output3");
        curout4_fp = registry.find_readable_field_t<unsigned int>("gomspace.curout.output4");
        curout5_fp = registry.find_readable_field_t<unsigned int>("gomspace.curout.output5");
        curout6_fp = registry.find_readable_field_t<unsigned int>("gomspace.curout.output6");

        output1_fp = registry.find_readable_field_t<bool>("gomspace.output.output1");
        output2_fp = registry.find_readable_field_t<bool>("gomspace.output.output2");
        output3_fp = registry.find_readable_field_t<bool>("gomspace.output.output3");
        output4_fp = registry.find_readable_field_t<bool>("gomspace.output.output4");
        output5_fp = registry.find_readable_field_t<bool>("gomspace.output.output5");
        output6_fp = registry.find_readable_field_t<bool>("gomspace.output.output6");

        wdt_i2c_time_left_fp = registry.find_readable_field_t<unsigned int>("gomspace.wdt_i2c_time_left");
        counter_wdt_i2c_fp = registry.find_readable_field_t<unsigned int>("gomspace.counter_wdt_i2c");
        counter_boot_fp = registry.find_readable_field_t<unsigned int>("gomspace.counter_boot");

        temp1_fp = registry.find_readable_field_t<signed int>("gomspace.temp.output1");
        temp2_fp = registry.find_readable_field_t<signed int>("gomspace.temp.output2");
        temp3_fp = registry.find_readable_field_t<signed int>("gomspace.temp.output3");
        temp4_fp = registry.find_readable_field_t<signed int>("gomspace.temp.output4");

        bootcause_fp = registry.find_readable_field_t<unsigned char>("gomspace.bootcause");
        battmode_fp = registry.find_readable_field_t<unsigned char>("gomspace.battmode");
        pptmode_fp = registry.find_readable_field_t<unsigned char>("gomspace.pptmode");
    }
};

void test_task_initialization() {
    TestFixture tf;
    //check that it is initialized correctly. tests pass
    TEST_ASSERT_EQUAL(1, tf.gs.hk->vboost[0]);
    TEST_ASSERT_EQUAL(2, tf.gs.hk->vboost[1]);
    TEST_ASSERT_EQUAL(3, tf.gs.hk->vboost[2]);

    TEST_ASSERT_EQUAL(4, tf.gs.hk->vbatt);

    TEST_ASSERT_EQUAL(5, tf.gs.hk->curin[0]);
    TEST_ASSERT_EQUAL(6, tf.gs.hk->curin[1]);
    TEST_ASSERT_EQUAL(7, tf.gs.hk->curin[2]);

    TEST_ASSERT_EQUAL(8, tf.gs.hk->cursun);

    TEST_ASSERT_EQUAL(9, tf.gs.hk->cursys);

    TEST_ASSERT_EQUAL(11, tf.gs.hk->curout[0]);
    TEST_ASSERT_EQUAL(12, tf.gs.hk->curout[1]);
    TEST_ASSERT_EQUAL(13, tf.gs.hk->curout[2]);
    TEST_ASSERT_EQUAL(14, tf.gs.hk->curout[3]);
    TEST_ASSERT_EQUAL(15, tf.gs.hk->curout[4]);
    TEST_ASSERT_EQUAL(16, tf.gs.hk->curout[5]);

    TEST_ASSERT_EQUAL(true, tf.gs.hk->output[0]);
    TEST_ASSERT_EQUAL(false, tf.gs.hk->output[1]);
    TEST_ASSERT_EQUAL(true, tf.gs.hk->output[2]);
    TEST_ASSERT_EQUAL(false, tf.gs.hk->output[3]);
    TEST_ASSERT_EQUAL(true, tf.gs.hk->output[4]);
    TEST_ASSERT_EQUAL(false, tf.gs.hk->output[5]);

    TEST_ASSERT_EQUAL(47, tf.gs.hk->wdt_i2c_time_left);

    TEST_ASSERT_EQUAL(51, tf.gs.hk->counter_wdt_i2c);

    TEST_ASSERT_EQUAL(55, tf.gs.hk->counter_boot);

    TEST_ASSERT_EQUAL(56, tf.gs.hk->temp[0]);
    TEST_ASSERT_EQUAL(57, tf.gs.hk->temp[1]);
    TEST_ASSERT_EQUAL(58, tf.gs.hk->temp[2]);
    TEST_ASSERT_EQUAL(59, tf.gs.hk->temp[3]);

    TEST_ASSERT_EQUAL(62, tf.gs.hk->bootcause);
    
    TEST_ASSERT_EQUAL(63, tf.gs.hk->battmode);
    
    TEST_ASSERT_EQUAL(64, tf.gs.hk->pptmode);
}

void test_task_execute() {
    TestFixture tf;
    tf.gs_controller->execute();

    //check that the statefields are set to their respective value(s) in hk struct 
    TEST_ASSERT_EQUAL(1, tf.vboost1_fp->get());
    TEST_ASSERT_EQUAL(2, tf.vboost2_fp->get());
    TEST_ASSERT_EQUAL(3, tf.vboost3_fp->get());

    TEST_ASSERT_EQUAL(4, tf.vbatt_fp->get());

    TEST_ASSERT_EQUAL(5, tf.curin1_fp->get());
    TEST_ASSERT_EQUAL(6, tf.curin2_fp->get());
    TEST_ASSERT_EQUAL(7, tf.curin3_fp->get());

    TEST_ASSERT_EQUAL(8, tf.cursun_fp->get());

    TEST_ASSERT_EQUAL(9, tf.cursys_fp->get());

    TEST_ASSERT_EQUAL(11, tf.curout1_fp->get());
    TEST_ASSERT_EQUAL(12, tf.curout2_fp->get());
    TEST_ASSERT_EQUAL(13, tf.curout3_fp->get());
    TEST_ASSERT_EQUAL(14, tf.curout4_fp->get());
    TEST_ASSERT_EQUAL(15, tf.curout5_fp->get());
    TEST_ASSERT_EQUAL(16, tf.curout6_fp->get());

    TEST_ASSERT_EQUAL(true, tf.output1_fp->get());
    TEST_ASSERT_EQUAL(false, tf.output2_fp->get());
    TEST_ASSERT_EQUAL(true, tf.output3_fp->get());
    TEST_ASSERT_EQUAL(false, tf.output4_fp->get());
    TEST_ASSERT_EQUAL(true, tf.output5_fp->get());
    TEST_ASSERT_EQUAL(false, tf.output6_fp->get());

    TEST_ASSERT_EQUAL(47, tf.wdt_i2c_time_left_fp->get());

    TEST_ASSERT_EQUAL(51, tf.counter_wdt_i2c_fp->get());

    TEST_ASSERT_EQUAL(55, tf.counter_boot_fp->get());

    TEST_ASSERT_EQUAL(56, tf.temp1_fp->get());
    TEST_ASSERT_EQUAL(57, tf.temp2_fp->get());
    TEST_ASSERT_EQUAL(58, tf.temp3_fp->get());
    TEST_ASSERT_EQUAL(59, tf.temp4_fp->get());

    TEST_ASSERT_EQUAL(62, tf.bootcause_fp->get());
    
    TEST_ASSERT_EQUAL(63, tf.battmode_fp->get());
    
    TEST_ASSERT_EQUAL(64, tf.pptmode_fp->get());
}

int test_control_task() {
    UNITY_BEGIN();
    RUN_TEST(test_task_initialization);
    RUN_TEST(test_task_execute);
    return UNITY_END();
}

#ifdef DESKTOP
int main() {
    return test_control_task();
}

#else

#include <Arduino.h>
void setup() {
    delay(2000);
    Serial.begin(9600);
    test_control_task();
}

#endif