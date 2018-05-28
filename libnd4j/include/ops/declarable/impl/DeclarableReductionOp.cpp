//
// Created by raver119 on 07.10.2017.
//

#include <ops/declarable/DeclarableReductionOp.h>
#include <ops/declarable/DeclarableOp.h>
#include <helpers/TAD.h>
#include <helpers/ShapeUtils.h>

#ifdef __CUDABLAS__
#include <cuda.h>
#include <cuda_runtime_api.h>
#include <cuda_runtime.h>
#endif

namespace nd4j {
    namespace ops {
        template <typename T>
        DeclarableReductionOp<T>::DeclarableReductionOp(int numInputs, int numOutputs, const char *opName, bool allowsInplace, int tArgs, int iArgs) : nd4j::ops::DeclarableOp<T>(numInputs, numOutputs, opName, allowsInplace, tArgs, iArgs) {
            //
        }

        template <typename T>
        DeclarableReductionOp<T>::~DeclarableReductionOp()  {
            //
        }


        template <typename T>
        nd4j::ShapeList* DeclarableReductionOp<T>::calculateOutputShape(nd4j::ShapeList* inputShape, nd4j::graph::Context<T>& block)  {
           // int numDims = INT_ARG(0);
            std::vector<int> dims;
            for (int e = 0; e < block.getIArguments()->size(); e++)
                dims.push_back(INT_ARG(e));

            if (dims.size() > 1)
                std::sort(dims.begin(), dims.end());

            // special case - output is scalar
            if (dims.size() == 0 || (dims.size() == 1 && dims.at(0) == MAX_INT)) {
                Nd4jLong* newShape;
                ALLOCATE(newShape, block.getWorkspace(), 8, Nd4jLong);

                newShape[0] = 2;
                newShape[1] = 1;
                newShape[2] = 1;
                newShape[3] = 1;
                newShape[4] = 1;
                newShape[5] = 0;
                newShape[6] = 1;
                newShape[7] = 99;

                return SHAPELIST(newShape);
            }

            shape::TAD tad(inputShape->at(0), dims.data(), dims.size());
            tad.createTadOnlyShapeInfo();

            Nd4jLong tadLength = shape::tadLength(inputShape->at(0), dims.data(), dims.size());
            Nd4jLong numTads = shape::length(inputShape->at(0)) /  tadLength;

            auto newShape = ShapeUtils<T>::evalReduceShapeInfo('c', dims, inputShape->at(0), false, true, block.getWorkspace());

            return SHAPELIST(newShape);
        }

        template class ND4J_EXPORT DeclarableReductionOp<float>;
        template class ND4J_EXPORT DeclarableReductionOp<float16>;
        template class ND4J_EXPORT DeclarableReductionOp<double>;
    }
}