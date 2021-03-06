/**
 * \file apriltagParameters.cpp
 * \attention This file is automatically generated and should not be in general modified manually
 *
 * \date MMM DD, 20YY
 * \author autoGenerator
 * Generated from apriltag.xml
 */

#include <vector>
#include <stddef.h>
#include "apriltagParameters.h"

/**
 *  Looks extremely unsafe because it depends on the order of static initialization.
 *  Should check standard if this is ok
 *
 *  Also it's not clear why removing "= Reflection()" breaks the code;
 **/

namespace corecvs {
#if 0
template<>
Reflection BaseReflection<ApriltagParameters>::reflection = Reflection();
template<>
int BaseReflection<ApriltagParameters>::dummy = ApriltagParameters::staticInit();
#endif
} // namespace corecvs 

SUPPRESS_OFFSET_WARNING_BEGIN


using namespace corecvs;

int ApriltagParameters::staticInit(corecvs::Reflection *toFill)
{
    if (toFill == NULL || toFill->objectSize != 0) {
        SYNC_PRINT(("staticInit(): Contract Violation in <ApriltagParameters>\n"));
         return -1;
    }

    toFill->name = ReflectionNaming(
        "Apriltag Parameters",
        "Apriltag Parameters",
        ""
    );

     toFill->objectSize = sizeof(ApriltagParameters);
     

    EnumField* field0 = new EnumField
        (
          ApriltagParameters::TAG_FAMILY_ID,
          offsetof(ApriltagParameters, mTag_family),
          2,
          "tag_family",
          "tag_family",
          "tag_family",
          new EnumReflection(8
          , new EnumOption(0,"tag16h5")
          , new EnumOption(1,"tag25h9")
          , new EnumOption(2,"tag36h11")
          , new EnumOption(3,"tagCircle21h7")
          , new EnumOption(4,"tagCircle49h12")
          , new EnumOption(5,"tagCustom48h12")
          , new EnumOption(6,"tagStandard41h12")
          , new EnumOption(7,"tagStandard52h13")
          )
        );
    field0->widgetHint=BaseField::COMBO_BOX;
    toFill->fields.push_back(field0);
    /*  */ 
    IntField* field1 = new IntField
        (
          ApriltagParameters::AT_THREADS_ID,
          offsetof(ApriltagParameters, mAt_threads),
          1,
          "at_threads",
          "at_threads",
          "at_threads"
        );
    toFill->fields.push_back(field1);
    /*  */ 
    BoolField* field2 = new BoolField
        (
          ApriltagParameters::AT_DEBUG_ID,
          offsetof(ApriltagParameters, mAt_debug),
          true,
          "at_debug",
          "at_debug",
          "at_debug"
        );
    field2->widgetHint=BaseField::CHECK_BOX;
    toFill->fields.push_back(field2);
    /*  */ 
    BoolField* field3 = new BoolField
        (
          ApriltagParameters::AT_QUIET_ID,
          offsetof(ApriltagParameters, mAt_quiet),
          false,
          "at_quiet",
          "at_quiet",
          "at_quiet"
        );
    field3->widgetHint=BaseField::CHECK_BOX;
    toFill->fields.push_back(field3);
    /*  */ 
    BoolField* field4 = new BoolField
        (
          ApriltagParameters::AT_REFINE_EDGES_ID,
          offsetof(ApriltagParameters, mAt_refine_edges),
          true,
          "at_refine_edges",
          "at_refine_edges",
          "at_refine_edges"
        );
    field4->widgetHint=BaseField::CHECK_BOX;
    toFill->fields.push_back(field4);
    /*  */ 
    DoubleField* field5 = new DoubleField
        (
          ApriltagParameters::AT_DECIMATE_ID,
          offsetof(ApriltagParameters, mAt_decimate),
          2,
          "at_decimate",
          "at_decimate",
          "at_decimate"
        );
    field5->widgetHint=BaseField::SPIN_BOX;
    field5->precision=2;
    toFill->fields.push_back(field5);
    /*  */ 
    DoubleField* field6 = new DoubleField
        (
          ApriltagParameters::BLUR_ID,
          offsetof(ApriltagParameters, mBlur),
          0,
          "blur",
          "blur",
          "blur"
        );
    field6->widgetHint=BaseField::SPIN_BOX;
    field6->precision=2;
    toFill->fields.push_back(field6);
    /*  */ 
    DoubleField* field7 = new DoubleField
        (
          ApriltagParameters::DECODE_SHARPENING_ID,
          offsetof(ApriltagParameters, mDecode_sharpening),
          0,
          "decode_sharpening",
          "decode_sharpening",
          "decode_sharpening"
        );
    field7->widgetHint=BaseField::SPIN_BOX;
    field7->precision=2;
    toFill->fields.push_back(field7);
    /*  */ 
    BoolField* field8 = new BoolField
        (
          ApriltagParameters::TRACE_ID,
          offsetof(ApriltagParameters, mTrace),
          false,
          "trace",
          "trace",
          "trace"
        );
    field8->widgetHint=BaseField::CHECK_BOX;
    toFill->fields.push_back(field8);
    /*  */ 
    IntField* field9 = new IntField
        (
          ApriltagParameters::MIN_CLUSTER_PIXELS_ID,
          offsetof(ApriltagParameters, mMin_cluster_pixels),
          5,
          "min_cluster_pixels",
          "min_cluster_pixels",
          "min_cluster_pixels"
        );
    toFill->fields.push_back(field9);
    /*  */ 
    IntField* field10 = new IntField
        (
          ApriltagParameters::MAX_NMAXIMA_ID,
          offsetof(ApriltagParameters, mMax_nmaxima),
          10,
          "max_nmaxima",
          "max_nmaxima",
          "max_nmaxima"
        );
    toFill->fields.push_back(field10);
    /*  */ 
    DoubleField* field11 = new DoubleField
        (
          ApriltagParameters::MAX_LINE_FIT_MSE_ID,
          offsetof(ApriltagParameters, mMax_line_fit_mse),
          10,
          "max_line_fit_mse",
          "max_line_fit_mse",
          "max_line_fit_mse"
        );
    field11->widgetHint=BaseField::SPIN_BOX;
    field11->precision=2;
    toFill->fields.push_back(field11);
    /*  */ 
    IntField* field12 = new IntField
        (
          ApriltagParameters::MIN_WHITE_BLACK_DIFF_ID,
          offsetof(ApriltagParameters, mMin_white_black_diff),
          5,
          "min_white_black_diff",
          "min_white_black_diff",
          "min_white_black_diff"
        );
    toFill->fields.push_back(field12);
    /*  */ 
    IntField* field13 = new IntField
        (
          ApriltagParameters::DEGLITCH_ID,
          offsetof(ApriltagParameters, mDeglitch),
          0,
          "deglitch",
          "deglitch",
          "deglitch"
        );
    toFill->fields.push_back(field13);
    /*  */ 
    ReflectionDirectory &directory = *ReflectionDirectoryHolder::getReflectionDirectory();
    directory[std::string("Apriltag Parameters")]= toFill;
   return 0;
}
int ApriltagParameters::relinkCompositeFields()
{
   return 0;
}

SUPPRESS_OFFSET_WARNING_END


